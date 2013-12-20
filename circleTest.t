terralib.require("prob")

local Vector = terralib.require("vector")
local Vec = terralib.require("linalg").Vec
local image = terralib.require("image")
local m = terralib.require("mem")
local util = terralib.require("util")
local rand = terralib.require("prob.random")
local ad = terralib.require("ad")

local Vec2d = Vec(double, 2)

local C = terralib.includecstring [[
#include <stdio.h>
#include <stdlib.h>
]]

local manifold = terralib.require("manifoldcpp")

local clamp = macro(function(x, lo, hi)
	return quote
		var y = x
		if y < lo then y = lo end
		if y > hi then y = hi end
	in
		y
	end
end)


local function circleModule()
	local radius = 0.4
	local temp = 0.001
	local angleWeakenFactor = 1000.0
	local Vec2 = Vec(real, 2)
	local ngaussian = macro(function(m, sd) return `gaussian(m, sd, {structural=false}) end)
	return terra()
		
		-- Prior
		-- var dotPos = Vec2.stackAlloc(ngaussian(0.5, 0.25), ngaussian(0.5, 0.01))
		-- var dotPos = Vec2.stackAlloc(ngaussian(0.5, 0.01), ngaussian(0.5, 0.25))
		var dotPos = Vec2.stackAlloc(ngaussian(0.5, 0.25), ngaussian(0.5, 0.25))

		-- -- Circle constraint
		-- var circCenter = Vec2.stackAlloc(0.5, 0.5)
		-- var distToCenter = dotPos:dist(circCenter)
		-- var err = distToCenter - radius
		-- err = err*err
		-- factor(-err/temp)

		-- GMM constraint
		var lg1 = [rand.gaussian_logprob(real)](dotPos(0), 0.3, 0.04)
		var lg2 = [rand.gaussian_logprob(real)](dotPos(0), 0.7, 0.04)
		var gmm = ad.math.log(0.5*ad.math.exp(lg1) + 0.5*ad.math.exp(lg2))
		factor(gmm)

		return dotPos
	end
end


local function stripLogprobs(samps)
	local terra strip()
		var points = [Vector(Vec2d)].stackAlloc()
		for i=0,samps.size do
			points:push(samps(i).value)
		end
		return points
	end
	return m.gc(strip())
end

local function doLLE(pointSamps)
	print("Doing LLE...")
	local inDim = 2
	local outDim = 1
	local numPoints = pointSamps.size
	local k = 20
	local terra dolle()
		var inData = [&double](C.malloc(numPoints*inDim*sizeof(double)))
		for i=0,numPoints do
			var inDataP = inData + i*inDim
			for d=0,inDim do
				inDataP[d] = pointSamps(i)(d)
			end
		end
		var outData = manifold.LLE(inDim, outDim, numPoints, k, inData)
		var outVector = [Vector(double)].stackAlloc(numPoints, 0.0)
		for i=0,numPoints do
			outVector(i) = outData[i]
		end
		C.free(outData)
		return outVector
	end
	return m.gc(dolle())
end

local function renderVideo(pointSamps)
	local dotRadius = 0.05
	local dotRadiusSq = dotRadius*dotRadius
	local ByteGrid = image.Image(uint8, 1)
	local terra renderDotImage(dotPos: &Vec2d, im: &ByteGrid)
		im:clear([ByteGrid.Color].stackAlloc(0))
		var w = im.width
		var h = im.height
		var xmin = clamp([int](w*(dotPos(0) - dotRadius)),0,w-1)
		var xmax = clamp([int](w*(dotPos(0) + dotRadius)),0,w-1)
		var ymin = clamp([int](h*(dotPos(1) - dotRadius)),0,h-1)
		var ymax = clamp([int](h*(dotPos(1) + dotRadius)),0,h-1)
		for y=ymin,ymax do
			var yd = [double](y)/h
			for x=xmin,xmax do
				var xd = [double](x)/w
				var p = Vec2d.stackAlloc(xd,yd)
				var dist2 = p:distSq(@dotPos)
				if dist2 < dotRadiusSq then
					im(x,y)(0) = 255
				end
			end
		end
		return im
	end
	io.write("Rendering video...")
	io.flush()
	local imageSize = 100
	local frameSkip = math.ceil(pointSamps.size / 1000.0)
	local terra renderFrames()
		var framename : int8[1024]
		var framenumber = 0
		var im = ByteGrid.stackAlloc(imageSize,imageSize)
		for i=0,pointSamps.size,frameSkip do
			C.sprintf(framename, "renders/movie_%06d.png", framenumber)
			framenumber = framenumber + 1
			var vecPtr = &pointSamps(i)
			renderDotImage(vecPtr, &im)
			[ByteGrid.save()](&im, image.Format.PNG, framename)
		end
		m.destruct(im)
	end
	renderFrames()
	util.wait("ffmpeg -threads 0 -y -r 30 -i renders/movie_%06d.png -c:v libx264 -r 30 -pix_fmt yuv420p renders/movie.mp4 2>&1")
	util.wait("rm -f renders/movie_*.png")
	print("done.")
end

---------------------------------------------

local numsamps = 2000
local burnin = 100
numsamps = numsamps + burnin
local kernel = HMC({numSteps=20, targetAcceptRate=0.65})
local terra doInference()
	return [mcmc(circleModule, kernel, {numsamps=numsamps, verbose=true, burnin=burnin})]
	-- return [forwardSample(circleModule, numsamps)]
end
local samples = m.gc(doInference())

local points = stripLogprobs(samples)

-- local embeddedPoints = doLLE(points)
-- -- Sort points so we can see how the embedding did
-- local struct PointWithEmbedding
-- {
-- 	point: Vec2d,
-- 	embedding: double
-- }
-- local terra comparePoints(p1: &opaque, p2: &opaque)
-- 	var p1v = [&PointWithEmbedding](p1)
-- 	var p2v = [&PointWithEmbedding](p2)
-- 	if p1v.embedding < p2v.embedding then
-- 		return -1
-- 	elseif p1v.embedding == p2v.embedding then
-- 		return 0
-- 	else
-- 		return 1
-- 	end
-- end
-- local sortedPoints = terralib.new(PointWithEmbedding[embeddedPoints.size])
-- for i=0,embeddedPoints.size-1 do
-- 	local pwe = terralib.new(PointWithEmbedding)
-- 	pwe.point = points:get(i)
-- 	pwe.embedding = embeddedPoints:get(i)
-- 	sortedPoints[i] = pwe
-- end
-- C.qsort(sortedPoints, points.size, terralib.sizeof(PointWithEmbedding), comparePoints:getpointer())
-- for i=0,points.size do
-- 	points:set(i, sortedPoints[i].point)
-- end

renderVideo(points)







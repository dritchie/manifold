local util = terralib.require("util")

local sourcefile = debug.getinfo(1, "S").source:gsub("@", "")
local dir = sourcefile:gsub("init.t", "")

util.wait(string.format("cd %s; make", dir))

local header = sourcefile:gsub("init.t", "lle.h")
local LLE = terralib.includec(header)

local lib = sourcefile:gsub("init.t", "liblle.so")
terralib.linklibrary(lib)

return LLE
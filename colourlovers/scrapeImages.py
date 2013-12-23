import sys
import os
import csv
import subprocess

def go(filename):
	path, ext = os.path.splitext(filename)
	if not os.path.exists(path):
		os.mkdir(path)
	f = open(filename, "r")
	csvfile = csv.reader(f, delimiter=",")
	rowsdone = 0
	for row in csvfile:
		# Skip header
		if rowsdone > 0:
			imageurl = row[5]
			sampid = row[0]
			temp = row[1]
			tempdir = os.path.join(path, temp)
			if not os.path.exists(tempdir):
				os.mkdir(tempdir)
			imagefilename = os.path.join(tempdir, sampid + ".png")
			cmd = "curl {0} -o {1}".format(imageurl, imagefilename)
			print cmd
			subprocess.call(cmd, shell=True)
		rowsdone += 1

if __name__ == "__main__":
	go(sys.argv[1])
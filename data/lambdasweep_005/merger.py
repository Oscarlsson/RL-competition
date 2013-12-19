#!/usr/bin/env python
import os
import pprint


dirs = [os.path.join(os.path.abspath('.'), x[0]) for x in os.walk(os.path.abspath('.'))]
dirs = dirs[1::]

outputdir = '000000-00-00-00-LibRLAgent-Episod100/'
if not os.path.isdir(outputdir):
    os.mkdir(outputdir)
    print "created dir"
print "Found directories" + str(dirs)

print "Starting."
for d in dirs:
    files = os.listdir(d)
    for f in files:
        with open(outputdir + f,"a+") as outputfile:
            with open(d + "/" + f,"r") as readresultfile:
                outputfile.write(readresultfile.read())

print "done"

#!/usr/bin/env python
import os
import shlex
import subprocess
import logging as lg
import ConfigParser
import stat
import time
import sys
import argparse

def MakeAll():
    lg.info("Running Makefile")
    os.system("make")
    lg.info("make done.")

def run_all(args):
    lg.info("Starting to run all enviroments")

    if args.E:
        environments = [args.E]
    else:
        environments = get_environments().split()
    lg.info("Environments found:" + str(environments))

    if args.A:
        agentname = args.A
    else:
        agentname = "./" + get_agent()
    print("Using agent " + agentname)

    if args.X:
        experimentname = './'+args.X
    else:
        experimentname = './'+get_experiment()
    print("Using experiment " + experimentname)

    timestr = time.strftime("%y%m%d-%H-%M-%S")
    printableagentname = agentname.split("/")[-1]
    printableexpname = experimentname.split("/")[-1]
    outputdir = get_outputdir() + timestr + "-" + printableagentname + "-" + printableexpname
    os.makedirs(outputdir)

    for environment in environments:
        for i in range(args.N):
            lg.info("Running: " + environment + " for the " + str(i) +"th time")
            print("Running: " + environment + " for the " + str(i) +"th time")
            run(environment, outputdir, agentname, args.output, experimentname)
    print "Output is found in " + outputdir

def run(environment, outputdir, agentname, output, experimentname):

    envname = environment.split("/")[-1].rstrip()
    resultfilename = outputdir + "/" + 'result' + envname 
    outputfilename = outputdir + "/" + 'output' + envname
    devnull = open('/dev/null', 'w')

    lg.info("* starting rl_glue")
    rlglue = subprocess.Popen(['rl_glue'], stdout=devnull)

    lg.info("* starting agent " + agentname)
    agent  = subprocess.Popen([agentname], stdout=devnull)

    envcmd = './'+str(environment)
    lg.info("* starting env with " + envcmd)
    subprocess.Popen([envcmd], shell=True, stdout=devnull)

    with open(outputfilename,'w') as outputfile:
        experiment = subprocess.Popen([experimentname, resultfilename], stdout=outputfile)
        experiment.communicate()

    if output:
        with open(outputfilename, 'r') as fin:
                print fin.read()
    devnull.close()

def get_environments():
    return read_config('environments')

def get_agent():
    return read_config('agent')

def get_experiment():
    return read_config('experiment')

def get_logfile():
    return read_config('logfile')

def get_outputdir():
    return read_config('outputdirectory')

def read_config(option):
    config = ConfigParser.RawConfigParser()
    config.read('../etc/runpyconfig')
    return config.get('runpy', option)


parser = argparse.ArgumentParser(description='This file will run an Agent on several environments and report the results to the directory ../outputs. Configuration is stored in ../etc/runpyconfig. ')
parser.add_argument('-A', metavar='agent',
                   help="Path to an executable agent. ",default=None, required=False)
parser.add_argument('-N', metavar='n', type=int,
                   help='Number of runs for EACH environment or with environment given by -E', default=1, required=False)
parser.add_argument('-E', metavar='env',
                   help='Path to an executable environment.', default=None, required=False)
parser.add_argument('-X', metavar='exp',
                   help='Path to an executable experiment.', default=None, required=False)
parser.add_argument('--output', help='Path to an executable environment.', action='store_true')

args = parser.parse_args()
log = get_outputdir() + get_logfile()
lg.basicConfig(filename=log, level=lg.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
MakeAll()
run_all(args)
# Close rl_glue if started

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
import numpy as np

def MakeAll():
    lg.info("Running Makefile")
    os.system("make")
    lg.info("make done.")

def run_all(args):
    lambdaarray, steparray = fix_lambdas_steps(args.l, args.s)
    environments = fix_envname(args.E)
    agentname = fix_agentname(args.A)
    experimentname = fix_expname(args.X)
    outputdir = fix_dirname(args.D, agentname, experimentname)

    for lambdas in lambdaarray:
        for stepsize in steparray:

            set_environment(lambdas, stepsize) 

            for environment in environments:
                for i in range(args.N):
                    lg.info("Running: " + environment + " for the " + str(i) +"th time")
                    print("Running: " + environment + " for the " + str(i) +"th time with lambda" + str(lambdas) + "and stepsize" + str(stepsize))
                    run(environment, outputdir, agentname, args.output, experimentname, lambdas, stepsize)
            print "Output is found in " + outputdir


def run(environment, outputdir, agentname, output, experimentname, lambdas, stepsize):

    envname = environment.split("/")[-1].rstrip()
    resultfilename = outputdir + "/" + 'result' + envname + "-" + str(lambdas) + "-" + str(stepsize)
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

def fix_lambdas_steps(lambdavalues, stepsizevalues):
    if len(args.l) == 3:
        lambdaarray = np.arange(lambdavalues[0], lambdavalues[1], lambdavalues[2])
    else:
        lambdaarray = lambdavalues

    if len(args.s) == 3:
        steparray = np.arange(stepsizevalues[0], stepsizevalues[1], stepsizevalues[2])
    else:
        steparray = stepsizevalues

    print "found lambdas:" + str(lambdaarray)
    print "found stepsizes: " + str(steparray)
    return lambdaarray, steparray

def set_environment(lambdas, stepsize):
    os.environ['LIBRLAGENT_LAMBDA'] = str(lambdas)
    os.environ['LIBRLAGENT_STEPSIZE'] = str(stepsize)

def fix_dirname(argument, agentname, experimentname):
    timestr = time.strftime("%y%m%d-%H-%M-%S")
    printableagentname = agentname.split("/")[-1]
    printableexpname = experimentname.split("/")[-1]
    if argument:
        outputdir = argument 
    else:
        outputdir = get_outputdir() + timestr + "-" + printableagentname + "-" + printableexpname
    print "Output directory is " + outputdir
    if not os.path.isdir(outputdir):
        print("Creating directory..")
        os.makedirs(outputdir)
    return outputdir

def fix_envname(argument):
    if argument:
        environments = [argument]
    else:
        environments = get_environments().split()
    print "Environments found:" + str(environments)
    lg.info("Environments found:" + str(environments))
    return environments

def fix_expname(argument):
    if argument:
        experimentname = './'+argument
    else:
        experimentname = './'+get_experiment()
    print("Using experiment " + experimentname)
    return experimentname

def fix_agentname(argument):
    if argument: 
        agentname = argument
    else:
        agentname = "./" + get_agent()
    print("Using agent " + agentname)
    return agentname

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




### ARGUMENTPARSER AND CALLS TO START
parser = argparse.ArgumentParser(description='This file will run an Agent on several environments and report the results to the directory ../outputs. Configuration is stored in ../etc/runpyconfig. ')
parser.add_argument('--l', metavar='n',type=float, nargs="+", help="Lambda value to the LibRLAgent. Either give one value --l 2 or three -l min max step. Default is 0", default=[0])
parser.add_argument('--s', metavar='n', type=float, nargs="+", help="Step size to the LibRLAgent. Either give one value --s 0 or three -l min max step. Default is 1", default=[1])
parser.add_argument('-A', metavar='agent',
                   help="Path to an executable agent. ",default=None, required=False)
parser.add_argument('-N', metavar='n', type=int,
                   help='Number of runs for EACH environment or with environment given by -E', default=1, required=False)
parser.add_argument('-E', metavar='environment',
                   help='Path to an executable environment.', default=None, required=False)
parser.add_argument('-X', metavar='experiment',
                   help='Path to an executable experiment.', default=None, required=False)
parser.add_argument('-D', metavar='dir',
                   help='Path to an output directory. Prefereably ../outputs/', default=None, required=False)

parser.add_argument('--output', help='Path to an executable environment.', action='store_true')

args = parser.parse_args()

log = get_outputdir() + get_logfile()
lg.basicConfig(filename=log, level=lg.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
MakeAll()
run_all(args)
# Close rl_glue if started

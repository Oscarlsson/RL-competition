#!/usr/bin/env python
import os
import shlex
import subprocess
import logging as lg
import ConfigParser

def MakeAll():
    lg.info("Running Makefile")
    os.system("make")
    lg.info("make done.")

def run_all():
    lg.info("Starting to run all enviroments")
    environments = get_environments()
    lg.info("Environments found:" + str(environments))
    for environment in environments:
        lg.info("Running: " + environment)
        run(environment)

def run(environment):
    agentname = './'+get_agent()
    experimentname = './'+get_experiment()
    lg.info("* starting rl_glue")
    rlglue = subprocess.Popen(['rl_glue'])
    lg.info("* starting agent " + agentname)
    agent  = subprocess.Popen([agentname])
    cmdenv = './'+str(environment)
    lg.info("* starting env with " + cmdenv)
    subprocess.Popen([cmdenv], shell=True)

    envname = environment.split("/")[2].rstrip()
    resultfilename = get_outputdir() + 'result' + envname 
    outputfilename = get_outputdir() + 'output' + envname
    with open(outputfilename,'w') as output:
        experiment = subprocess.Popen([experimentname, resultfilename], stdout=output)
        experiment.communicate()

def get_environments():
    arg = ['find environments -executable -type f']
    executablefiles = subprocess.Popen(arg, shell=True, stdout=subprocess.PIPE)
    return executablefiles.stdout

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

log = get_outputdir() + get_logfile()
lg.basicConfig(filename=log, level=lg.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
MakeAll()
run_all()
# Close rl_glue if started

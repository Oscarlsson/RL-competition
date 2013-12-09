#!/usr/bin/env python
import os
import subprocess

def MakeAll():
    print "---------- Running Makefile ----------"
    os.system("make")
    print "----------                  ----------"

def run_all():
    print "----------     Starting run -----------"
    print "----------  RL-glue started ----------"
    rlglue = subprocess.Popen(['rl_glue'])
    print "----------  Agent   started ----------"
    agent  = subprocess.Popen(['./MyAgent'])
    
    
    print "----------  Experiment   started ----------"
    with open('outputfile','w') as output:
        experiment = subprocess.Popen(['./MyExperiment'], stdout=output)
        experiment.communicate()
    #TODO: Open all environments
    # os.listdir


MakeAll()
run_all()

# Run #
# One environment-test
    # Start RL_GLUE
    # Start Experiment
    # Start AGENT
    # Run one environment in /environtments

    # Print/Save output
    # at least experiments..


#! /bin/bash

## set up the host address and port!
export RLGLUE_PORT=4137
export RLGLUE_HOST="localhost"

## run the experiment
rl_glue >rl_glue.out &
./BanditEnvironment >environment.out &
./SarsaAgent >agent.out &
./SampleExperiment >experiment.out

#grep "episode reward" experiment.out >random.reward
grep "episode reward" experiment.out >sarsa.reward

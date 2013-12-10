#! /bin/bash

## set up the host address and port!
export RLGLUE_PORT=4137
export RLGLUE_HOST="localhost"

## run the experiment
./LoopEnvironment >environment.out &
./SarsaAgent >agent.out &
./SampleExperiment >experiment.out &
rl_glue >rl_glue.out 


#grep "episode reward" experiment.out >random.reward
grep "episode reward" experiment.out >loop-sarsa.reward

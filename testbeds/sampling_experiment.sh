#! /bin/bash

## set up the host address and port!
export RLGLUE_PORT=4137
export RLGLUE_HOST="localhost"

## run the experiment
#./LoopEnvironment >environment.out &
./ChainEnvironment >environment.out &
##./TicTacToe >environment.out &
./SampleExperiment >experiment.out &
rl_glue >rl_glue.out&
./SamplingAgent >agent.out 
##gdb --args ./sample_agent_rl_glue 



#grep "episode reward" experiment.out >random.reward
##grep "episode reward" experiment.out >loop-sampling.reward
grep "episode reward" experiment.out >chain-sampling.reward

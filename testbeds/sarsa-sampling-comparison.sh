#! /bin/bash

## set up the host address and port!
export RLGLUE_PORT=4137
export RLGLUE_HOST="localhost"

for env in Bandit ContextBandit Chain Loop TicTacToe Mines
do
## run the experiment
	for alg in Sarsa Sampling
	do
		./${env}Environment >environment.out &
		./${alg}Agent >agent.out &
		./SampleExperiment >experiment.out &
		rl_glue >rl_glue.out 
		
		grep "episode reward" experiment.out >${env}-${alg}.reward
		grep "total reward" experiment.out >${env}-${alg}.utility
	done
done

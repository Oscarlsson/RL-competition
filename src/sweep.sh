#!/bin/bash

rm -rf ../outputs/*

for lambda in `seq 0.5 0.1 0.9 | tr ',' '.'`;
do
    export LIBRLAGENT_LAMBDA=$lambda
    for stepsize in `seq 0.05 0.1 0.55 | tr ',' '.'`;
    do
        export LIBRLAGENT_STEPSIZE=$stepsize
        echo "$lambda; $stepsize; " | tr -d '\n'
        ./run.py -N 10 | grep -Po '(?<=Output is found in ).*'
    done 
done 
unset LIBRLAGENT_LAMBDA
unset LIBRLAGENT_STEPSIZE

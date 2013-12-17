#!/bin/sh

export RLGLUE_PORT=20003
export RLGLUE_HOST="cse-1067630.cse.chalmers.se"
time ./agent/LibRLAgent >> ../data/test_output.txt


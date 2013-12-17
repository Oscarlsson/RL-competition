RL-competition
==============

This project is part of a competition in the course "Decision making under uncertainty"
homepage: http://www.cse.chalmers.se/~chrdimi/teaching/optimal_decisions/index.html

Team: Oscar Carlsson, John Karlsson, Oskar Lindgren

Keywords: Discrete state space, Implemented agent using Sarsa-lambda, KL-UCB, option to vary parameters


This plot illustrates how our agent performs on average over 15 runs in a experiment with 1000 episodes. 
![Plot] (https://raw.github.com/Oscarlsson/RL-competition/master/data/100episodes_50runs.png "Our agent against several different environments")

Install
=============

* RL-glue 3.04

see folder RL-GLUE and install.sh. 

* C++11

needed for some of the environments

* matplotlib
* pandas 0.12

used in run.py and matplotlib

# Run the agent

The interesting code is found in`src`

run using `cd src; ./run.py` to run the default setup defined in etc/runpyconfig
Run `./run.py -h` for a list of available arguments

output is stored in `../outputs` and you can generate pretty plots using
`./plotresults.py -D ../outputs/<yourrun>`


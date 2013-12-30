RL-competition
==============

This project is part of a competition in the course "Decision making under uncertainty"
homepage: http://www.cse.chalmers.se/~chrdimi/teaching/optimal_decisions/index.html

Team: Oscar Carlsson, John Karlsson, Oskar Lindgren

This project is explained in detail in the report found in ``report``
Keywords: Discrete state space, Implemented agent using Sarsa-lambda, KL-UCB, option to vary parameters


This plot illustrates how our agent performs on average over 15 runs in a experiment with 1000 episodes. 
![Plot] (https://raw.github.com/Oscarlsson/RL-competition/master/data/100episodes_50runs.png "Our agent against several different environments")

Install
=============

* RL-glue 3.04

see ``RL-GLUE/install.sh`` and [rl-glue] (http://glue.rl-community.org/wiki/Main_Page "rl-glue") for more information

* C++11 (Some environment)
* matplotlib (Run.py and plotresults)
* pandas 0.12 (Run.py and plotresults)


# Run the agent

The code containing the ``Agent Experiment Environments`` is found in`src`

run using `cd src; ./run.py` to run the default setup defined in etc/runpyconfig
Run `./run.py -h` for a list of available arguments

output is stored in `../outputs` and you can generate pretty plots using
`./plotresults.py -D ../outputs/<yourrun>`


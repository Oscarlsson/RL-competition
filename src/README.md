Run.py
=======
Starts rl_glue, experiment and agents and runs it agains either all environments or the environment chosen by -E

How to run the standard agent on Loop-environment, 100 time with lambda 1 and step size 0

```python
./run.py -E environments/loop/LoopEnvironment -N 100 --l 1 --s 0
```


Plotresults
=======
Run using `./plotresults -D ../outputs/<dirname>` and it will plot the results


Agent
========

Sarsa lambda, KL-UCB, Tiebraker etc.


Experiments
=======

# MyExperiment
Nr episodes, save to csv

# ChristosExperiment
Nr episodes, save to csv

Environments
=======

# Bandit
States: Rewards: Actions:

Description: 

# Contextbandits

States: Rewards: Actions:

Description: 

# Loop

States: Rewards: Actions:

Description: 

# Mines

States: Rewards: Actions:

Description: 

# Tictactoe

States: Rewards: Actions:

Description: 

# Connectfour

States: Rewards: Actions:

Description: 

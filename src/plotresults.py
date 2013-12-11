#!/usr/bin python
import pandas as pd
import matplotlib.pyplot as plt

csvfile = '../outputs/131211-15-54-28-LibRLAgent/resultMyEnvironment'
csvdata = pd.read_csv(csvfile, index_col=0, header=None, names=range(0,525,25))
#print csvdata.index

print csvdata.loc['mean']
print csvdata.loc['std']

csvdata.loc['mean'].plot()
plt.show()

# run
# same Environment
# same Agent
# same Experiment
# n times

# => 100 csv, take mean, plot

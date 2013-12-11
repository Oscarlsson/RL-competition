#!/usr/bin python
import pandas as pd
import matplotlib.pyplot as plt

csvfile = 'result'
#csvfile = '../outputs/131211-15-54-28-LibRLAgent/resultMyEnvironment'
csvdata = pd.read_csv(csvfile, index_col=0, header=None, names=range(0,525,25))

#print csvdata[0].loc['mean']
#print csvdata[0].loc['mean'].mean()
meandata = csvdata.loc['mean']
meandata.mean().plot()
plt.ylabel('#Episodes')
plt.xlabel('Average reward')
plt.title('Environment z with x runs using agent y')
#print csvdata.index

#print csvdata.loc['mean']
#print csvdata.loc['std']

#csvdata.loc['mean'].plot()
plt.show()

# run
# same Environment
# same Agent
# same Experiment
# n times

# => 100 csv, take mean, plot

import pandas as pd
import matplotlib.pyplot as plt

csvfile = '../data/lambdadataa'

data = pd.read_table(csvfile, sep=' ', header=None, index_col=[0,1])
indexes = set([a for (a,b) in data.index])

for index in indexes:
    plotdata = data.loc[index]
    plotdata[2].plot()

plt.xlabel("Lambda")
plt.ylabel("Cumulative reward")
plt.legend(indexes, loc='best')
plt.show()

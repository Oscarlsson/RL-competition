import pandas as pd
import matplotlib.pyplot as plt
import sys

# ../outputs/131218-12-27-39-LibRLAgent-Episod100/finalOutput
csvfile = sys.argv[1] 

data = pd.read_table(csvfile, sep=' ', header=None, index_col=[0,1])
print data
indexes = set([a for (a,b) in data.index])
print indexes

for index in indexes:
    plotdata = data.loc[index]
    plotdata[3].plot()

plt.xlabel("Lambda")
plt.ylabel("Cumulative reward")
plt.legend(indexes, loc='best')
plt.show()

#!/usr/bin/env python

import pandas as pd
import matplotlib.pyplot as plt
import sys

# ../outputs/131218-12-27-39-LibRLAgent-Episod100/finalOutput
csvfile = sys.argv[1] 

data = pd.read_table(csvfile, sep=' ', header=None, index_col=[0,1])
indexes = set([a for (a,b) in data.index])

for index in indexes:
    plotdata = data.loc[index]
    # Normal plot
    #plotdata[3].sort_index().plot()
    # Normalized
    plotdata = (plotdata[3])#-plotdata[3].min())
    (plotdata/plotdata.abs().max()).sort_index().plot()

plt.xlabel("Change i reward for different values of lambda")
plt.xlabel("Lambda")
plt.ylabel("Cumulative reward")
plt.legend(indexes, loc='best')
plt.show()

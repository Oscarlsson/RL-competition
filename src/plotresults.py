#!/usr/bin/env python
import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os

def plot(csvfile):
    csvdata = pd.read_csv(csvfile, index_col=0, header=None, names=range(0,525,25))
    meandata = csvdata.loc['mean']
    if meandata.ndim == 1:
        meandata.plot()
    else:
        meandata.mean().plot()

parser = argparse.ArgumentParser(description='Plotting the average mean of a result-output. Argument is the directory')
parser.add_argument('-D', metavar='dir', required=True,
                   help="Directory where result files are found.",default=None)
args = parser.parse_args()

results = [filename for filename in os.listdir(args.D) if filename.startswith('result')]
plt.figure()
plt.ylabel('#Episodes')
plt.xlabel('Average reward')
plt.title('Environment z with x runs using agent y')
print results
for result in results:
    plot(args.D+"/"+result)
plt.legend(results)
plt.show()

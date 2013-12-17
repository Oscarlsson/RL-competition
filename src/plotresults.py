#!/usr/bin/env python
import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os
import numpy as np

def plot(csvdata, agent):

    meandata = csvdata.loc['mean']
    dim = meandata.ndim
    print csvfile

    if dim == 1:
        plt.title('With ' + str(dim) + ' runs using ' + agent)
        meandata.cumsum().plot()
    else:
        plt.title('With ' + str(len(meandata)) + ' runs using ' + agent)
        meandata.mean().cumsum().plot()

def printable(result):
    splitlist = result.split("-")
    name= splitlist[0].rsplit("Environment")[0].rsplit("result")[1]
    return name + " l:" + splitlist[1] +" s:" + splitlist[2]

parser = argparse.ArgumentParser(description='Plotting the average mean of a result-output. Argument is the directory')
parser.add_argument('-D', metavar='dir', required=True,
                   help="Directory where result files are found.",default=None)
args = parser.parse_args()
### Parser ends

results = [filename for filename in os.listdir(args.D) if filename.startswith('result')]
plt.figure()
plt.xlabel('#Episodes')
plt.ylabel('Average reward')
print results

if args.D.endswith("/"):
    args.D = args.D[0:-1]

experiment = args.D.split("/")[-1].split("-")[-1] #last elem

# New color each plot
# Might look the same
colormap = plt.cm.gist_ncar
num_plots = len(results)
plt.gca().set_color_cycle([colormap(i) for i in np.linspace(0, 0.9, num_plots)])

for result in results:
    csvfile = args.D + "/" + result
    if experiment == 'MyExperiment':
        csvdata = pd.read_csv(csvfile, index_col=0, header=None, names=range(0,525,25))
    elif experiment.endswith("100"):
        csvdata = pd.read_csv(csvfile, index_col=0, header=None, names=range(0,101))
    elif experiment.endswith("1000"):
        csvdata = pd.read_csv(csvfile, index_col=0, header=None, names=range(0,1001))

    agent = csvfile.split("/")[-2]
    plot(csvdata, agent)

results= [ printable(result) for result in results]
plt.legend(results, loc='best')
plt.savefig(args.D+"/plot.png")
plt.show()

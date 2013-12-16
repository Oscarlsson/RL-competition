#!/usr/bin/env python
import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os
import numpy as np

def plot_christos(csvfile):
    csvdata = pd.read_csv(csvfile, index_col=0, header=None, names=range(0,101))
    meandata = csvdata.loc['mean']
    dim = meandata.ndim
    agent = csvfile.split("/")[-2]
    print csvfile

    if dim == 1:
        plt.title('With ' + str(dim) + ' runs using ' + agent)
        meandata.cumsum().plot()
    else:
        plt.title('With ' + str(len(meandata)) + ' runs using ' + agent)
        printdata = meandata.mean().cumsum().plot()


def plot_exp(csvfile):
    csvdata = pd.read_csv(csvfile, index_col=0, header=None, names=range(0,525,25))
    meandata = csvdata.loc['mean']
    dim = meandata.ndim
    agent = csvfile.split("/")[-2]
    print csvfile
    if dim == 1:
        plt.title('With ' + str(dim) + ' runs using ' + agent)
        meandata.plot()
    else:
        plt.title('With ' + str(len(meandata)) + ' runs using ' + agent)
        meandata.mean().plot()

def printable(result):
    splitlist = result.split("-")
    name= splitlist[0].rsplit("Environment")[0].rsplit("result")[1]
    return name + " l:" + splitlist[1] +" s:" + splitlist[2]

parser = argparse.ArgumentParser(description='Plotting the average mean of a result-output. Argument is the directory')
parser.add_argument('-D', metavar='dir', required=True,
                   help="Directory where result files are found.",default=None)
args = parser.parse_args()

results = [filename for filename in os.listdir(args.D) if filename.startswith('result')]
plt.figure()
plt.xlabel('#Episodes')
plt.ylabel('Average reward')
print results

if args.D.endswith("/"):
    args.D = args.D[0:-1]

# Fulhack?
experiment = args.D.split("/")[-1].split("-")[-1] #last elem

if experiment == 'MyExperiment':
    plotf = plot_exp
else:
    plotf = plot_christos

colormap = plt.cm.gist_ncar
num_plots = len(results)
plt.gca().set_color_cycle([colormap(i) for i in np.linspace(0, 0.9, num_plots)])

for result in results:
    plotf(args.D+"/"+result)

results= [ printable(result) for result in results]
plt.legend(results, loc='best')
plt.savefig(args.D+"/plot.png")
plt.show()

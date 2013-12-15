#!/usr/bin/env python
import pandas as pd
import matplotlib.pyplot as plt
import argparse
import os

def plot_christos(csvfile):
    pass
    #    csvdata = pd.read_csv(csvfile, index_col=0, header=None, names=range(0,525, 25))
#    csvdata.loc['mean'][range(0,125,25)].plot()


def plot_myexp(csvfile):
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
experiment = args.D.split("/")[-1].split("-")[-1]
if experiment == 'ChristosExperiment':
    plotf = plot_christos
else:
    plotf = plot_myexp

for result in results:
    plotf(args.D+"/"+result)

plt.legend(results, loc=4)
plt.savefig(args.D+"/plot.png")
plt.show()

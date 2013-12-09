#If RL-Glue Core or the C/C++ Codecs are not installed to default locations
#You may need to use set the CFLAGS and LDFLAGS. 

LDFLAGS := $(LDFLAGS) -lm -lrlutils -lrlgluenetdev
CFLAGS := $(CFLAGS) -std=c++11 -Wall -pedantic -g -O3
CC := $(CXX) #

all: BanditEnvironment SarsaAgent MinesEnvironment SampleExperiment
#distr


### environments

BanditEnvironment: BanditEnvironment.o
	$(CC) BanditEnvironment.o -lrlenvironment $(LDFLAGS) -o BanditEnvironment

BanditEnvironment.o: BanditEnvironment.cc
	$(CC)  $(CFLAGS) -c BanditEnvironment.cc -o BanditEnvironment.o 

MinesEnvironment: MinesEnvironment.o
	$(CC) MinesEnvironment.o -lrlenvironment $(LDFLAGS) -o MinesEnvironment

MinesEnvironment.o: MinesEnvironment.cc
	$(CC)  $(CFLAGS) -c MinesEnvironment.cc -o MinesEnvironment.o 

### agents

SarsaAgent: SarsaAgent.o
	$(CC) SarsaAgent.o -lrlagent $(LDFLAGS) -o SarsaAgent

SarsaAgent.o: SarsaAgent.cc
	$(CC)  $(CFLAGS) -c SarsaAgent.cc -o SarsaAgent.o 


### experiments

SampleExperiment: SampleExperiment.o
	$(CC) SampleExperiment.o -lrlexperiment $(LDFLAGS) -o SampleExperiment

SampleExperiment.o: SampleExperiment.cc
	$(CC)  $(CFLAGS) -c SampleExperiment.cc -o SampleExperiment.o 

#distr: 
#	tar cvfj environments.tar.bz2 *.cc Makefile *.sh

clean:
	rm -rf *.o *~ SarsaAgent MinesEnvironment BanditEnvironment SampleExperiment







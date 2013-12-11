#If RL-Glue Core or the C/C++ Codecs are not installed to default locations
#You may need to use set the CFLAGS and LDFLAGS. 

LDFLAGS := $(LDFLAGS) -lm -lrlutils -lrlgluenetdev
CFLAGS := $(CFLAGS) -std=c++11 -Wall -pedantic -g -O3
CC := g++

all: distr ChainEnvironment ContextBanditEnvironment BanditEnvironment SarsaAgent MinesEnvironment SampleExperiment LoopEnvironment TicTacToeEnvironment


################### environments #####################

## bandits ##
BanditEnvironment: BanditEnvironment.o
	$(CC) BanditEnvironment.o -lrlenvironment $(LDFLAGS) -o BanditEnvironment

BanditEnvironment.o: BanditEnvironment.cc
	$(CC)  $(CFLAGS) -c BanditEnvironment.cc -o BanditEnvironment.o 

## context bandits ##
ContextBanditEnvironment: ContextBanditEnvironment.o
	$(CC) ContextBanditEnvironment.o -lrlenvironment $(LDFLAGS) -o ContextBanditEnvironment

ContextBanditEnvironment.o: ContextBanditEnvironment.cc
	$(CC)  $(CFLAGS) -c ContextBanditEnvironment.cc -o ContextBanditEnvironment.o 

## mines ##
MinesEnvironment: MinesEnvironment.o
	$(CC) MinesEnvironment.o -lrlenvironment $(LDFLAGS) -o MinesEnvironment

MinesEnvironment.o: MinesEnvironment.cc
	$(CC)  $(CFLAGS) -c MinesEnvironment.cc -o MinesEnvironment.o 

## mines ##
TicTacToeEnvironment: TicTacToeEnvironment.o
	$(CC) TicTacToeEnvironment.o -lrlenvironment $(LDFLAGS) -o TicTacToeEnvironment

TicTacToeEnvironment.o: TicTacToeEnvironment.cc
	$(CC)  $(CFLAGS) -c TicTacToeEnvironment.cc -o TicTacToeEnvironment.o 


## loop ##
LoopEnvironment: LoopEnvironment.o
	$(CC) LoopEnvironment.o -lrlenvironment $(LDFLAGS) -o LoopEnvironment

LoopEnvironment.o: LoopEnvironment.cc
	$(CC)  $(CFLAGS) -c LoopEnvironment.cc -o LoopEnvironment.o 


## chain ##
ChainEnvironment: ChainEnvironment.o
	$(CC) ChainEnvironment.o -lrlenvironment $(LDFLAGS) -o ChainEnvironment

ChainEnvironment.o: ChainEnvironment.cc
	$(CC)  $(CFLAGS) -c ChainEnvironment.cc -o ChainEnvironment.o 




#################### agents #####################

## Sarsa Agent
SarsaAgent: SarsaAgent.o
	$(CC) SarsaAgent.o -lrlagent $(LDFLAGS) -o SarsaAgent

SarsaAgent.o: SarsaAgent.cc
	$(CC)  $(CFLAGS) -c SarsaAgent.cc -o SarsaAgent.o 


################ experiments ####################

SampleExperiment: SampleExperiment.o
	$(CC) SampleExperiment.o -lrlexperiment $(LDFLAGS) -o SampleExperiment

SampleExperiment.o: SampleExperiment.cc
	$(CC)  $(CFLAGS) -c SampleExperiment.cc -o SampleExperiment.o 

distr: 
	tar cvfj environments.tar.bz2 *.cc Makefile *.sh

clean:
	rm -rf *.o *~ SarsaAgent TicTacToeEnvironment MinesEnvironment ContextBanditEnvironment ChainEnvironment LoopEnvironment BanditEnvironment SampleExperiment








# This makefile is responsible for calling the makefiles for
# 		Agent
# 		Environments
# 		Experiments
 
 LDFLAGS := $(LDFLAGS) -lm -lrlutils -lrlgluenetdev
 CFLAGS := $(CFLAGS) -std=c++11 -Wall -pedantic -g -O3


all: 	
	CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" make -f agent/Makefile.agent 
	CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" make -f experiments/Makefile.experiment
	CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" make -f environments/Makefile.environment 
clean:
	make -f agent/Makefile.agent  clean
	make -f experiments/Makefile.experiment clean
	make -f environments/Makefile.environment clean

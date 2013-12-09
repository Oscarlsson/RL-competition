
# This makefile is responsible for calling the makefiles for
# 		Agent
# 		Environments
# 		Experiments
 
 LDFLAGS := $(LDFLAGS) -lm -lrlutils -lrlgluenetdev
 CFLAGS := $(CFLAGS) -std=c++11 -Wall -pedantic -g -O3


all: 	
	CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" make -f Makefile.agent 
	CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" make -f environments/Makefile.environment 
	CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" make -f Makefile.experiment

clean:
	make -f Makefile.agent  clean
	make -f environments/Makefile.environment clean
	make -f Makefile.experiment clean

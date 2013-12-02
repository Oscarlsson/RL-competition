#!/bin/sh 
tar -zxf rlglue-3.04.tar.gz
tar -zxf c-codec-2.0.tar.gz 
cd rlglue-3.04
./configure; make; sudo make install
export LD_LIBRARY_PATH=/usr/local/lib
cd ../c-codec-2.0
./configure; make; sudo make install
cd ../c-codec-2.0/examples/mines-sarsa-example-cpp
make

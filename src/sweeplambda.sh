#!/bin/sh

./run.py -N 10 -E environments/bandits/BanditEnvironment --l 0 1 0.1
./run.py -N 10 -E environments/chain/ChainEnvironment --l 0 1 0.1
./run.py -N 10 -E environments/connectfour/ConnectfourEnvironment --l 0 1 0.1
./run.py -N 10 -E environments/contextbandit/ContextBanditEnvironment --l 0 1 0.1
./run.py -N 10 -E environments/loop/LoopEnvironment --l 0 1 0.1
./run.py -N 10 -E environments/mines/MinesEnvironment --l 0 1 0.1
./run.py -N 10 -E environments/tictactoe/TictactoeEnvironment --l 0 1 0.1

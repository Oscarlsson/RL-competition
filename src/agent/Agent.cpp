#include "Agent.hpp"

#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>      

Agent::Agent(int nStates, int nActions, double gamma, double lambda)
            : nStates(nStates),
              nActions(nActions),
              gamma(gamma),
              lambda(lambda)
{
    qTable = new double*[nStates];
    traces = new double*[nStates];
    for (int s = 0; s < nStates; ++s)
    {
        qTable[s] = new double[nActions];
        traces[s] = new double[nActions];
    }
    std::cout << "Initializing agent with parameters:" << std::endl
              << "\tnStates : "  << nStates  << std::endl
              << "\tnActions : " << nActions << std::endl
              << "\tgamma : "    << gamma    << std::endl
              << "\tlambda : "   << lambda   << std::endl;
}




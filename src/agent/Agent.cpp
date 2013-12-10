#include "Agent.hpp"

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>      
#include <time.h>

Agent::Agent(int nStates, int nActions, double gamma, double lambda)
            : nStates(nStates),
              nActions(nActions),
              gamma(gamma),
              lambda(lambda)
{
    srand(time(0));
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

int randInRange(int max);
int Agent::step(double reward, int newState)
{
  //std::cout << "Observed reward " << reward << " in state "
  //          << newState << std::endl;
    
    // A = previous action
    // S = previous state
    //
    // A' = next action from SomeNicePolicyClass (EpsilonGreedy)
    // S' = this state (int newState)
    //
    //
    // TODO: spara nonzero Z-index i std::vector
    // uppdatera d, Z samt Q(s,a), Z(s,a) forall a,s
    // TODO: loopa endast genom sparse matris (lista på nonzero index ovan)
    //  
    //
    // return A'
    return randInRange(nActions - 1);
}



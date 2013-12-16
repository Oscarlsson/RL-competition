#include "Agent.hpp"
#include "UCB1Policy.hpp"

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>      
#include <time.h>
#include <cfloat>

using namespace std;

double runif()
{
    return ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
}

Agent::Agent(int nStates, int nActions, double gamma, double lambda,
             double stepSize, double minReward, double maxReward)
: nStates(nStates), nActions(nActions), gamma(gamma), lambda(lambda),
  stepSize(stepSize), t(0), e(0), minReward(minReward), maxReward(maxReward)
{
    srand(time(0));
    qTable = new double*[nStates];
    traces = new double*[nStates];
    counts = new double*[nStates];
    for (int s = 0; s < nStates; ++s)
    {
        qTable[s] = new double[nActions];
        traces[s] = new double[nActions];
        counts[s] = new double[nActions];
       // memset(qTable[s], maxReward, sizeof(qTable[s])*nActions);
       // memset(counts[s], 1.0,         sizeof(counts[s])*nActions);
        for (int a = 0; a < nActions; ++a)
        {
            qTable[s][a] = maxReward;
            counts[s][a] = 1;
        }
    }
    Qmin = minReward;
    Qmax = maxReward;

    cerr << "Initializing agent with parameters:" << endl
              << "\tnStates : "  << this->nStates  << endl
              << "\tnActions : " << this->nActions << endl
              << "\tgamma : "    << this->gamma    << endl
              << "\tlambda : "   << this->lambda   << endl
              << "\tstepsize : " << this->stepSize << endl
              << "\tpolicy.c : " << policy.c       << endl
              << "\ttriebreaker : " << policy.tiebreaker       << endl
              << "\treward : ["  << minReward << "," << maxReward << "]"
              << endl;
}

Agent::~Agent()
{
    std::cout << "~Agent()" << endl;
    for (int i = 0; i < nStates; ++i)
    {
        delete [] qTable[i];
        delete [] traces[i];
        delete [] counts[i];
    }
}

/*
 * This method implements the Tabular Sarsa(lambda) from Figure 7.11 (Sutton)
 */
int Agent::step(int lastState, int lastAction, double reward, int thisState)
{
    const int history_size = 5000;
    const int history_incr = 1000;
    static vector<int> history_S(history_size);
    static vector<int> history_A(history_size);

    // Take action A, observe R, S'
    // ... is given already as:
    int S = lastState;
    int A = lastAction;
    int S2= thisState;  // S'

    if (t == history_S.capacity())
        history_S.resize(t + history_incr);
    if (t == history_A.capacity())
        history_A.resize(t + history_incr);

    history_S[t] = S;
    history_A[t] = A;

//  cerr << " >> " ;
//  for (int a = 0; a < nActions; ++a)
//      cerr << counts[S][a] << " ";
//  cerr << endl;
    if (reward == -10000000)
    {
        cerr << "***    qMin: " << Qmin << " qMax: " << Qmax << endl;
        double localTime = 0;
        for (int a = 0; a < nActions; ++a)
            localTime += counts[S][a];
        for (int a = 0; a < nActions; ++a)
            cerr << qTable[S][a] << " ";
        cerr << endl << "\t";
        for (int a = 0; a < nActions; ++a)
        {
            double u = policy.newtonRapson((qTable[S][a]-Qmin)/(Qmax-Qmin),
                    localTime, counts[S][a]);
            cerr << u << " (" << counts[S][a] << "), ";
        }
        cerr << endl;
    }

//  double lambda = this->lambda / sqrt(t+1);
//  double stepSize = this->stepSize / sqrt(t+1);

//  double lambda = this->lambda / sqrt(t+1);
//  double stepSize = this->stepSize / sqrt(t+1);

    // Choose A2 from S2 using policy derived from Q
    // UCB1
    int A2 = policy.sample_action(S2, t, qTable, counts, nActions, history_S,
                                   lambda, Qmin, Qmax);
    
    double delta = reward + gamma * qTable[S2][A2] - qTable[S][A];
    traces[S][A] += 1;

    for (int s = 0; s < nStates; ++s)
    {
        for (int a = 0; a < nActions; ++a)
        {
            qTable[s][a] += stepSize * delta * traces[s][a];
            counts[s][a] += traces[s][a]; 
            traces[s][a]  = gamma * lambda * traces[s][a];
            
            if (qTable[s][a]<Qmin)
                Qmin = qTable[s][a];
            if (qTable[s][a]>Qmax)
                Qmax = qTable[s][a];
        }
    }

    ++t;
    return A2;
}

void Agent::start()
{
    t = 0;
    ++e;
    cerr << "  Ep #:\t" << e << "\r";
    cerr.flush();
    for (int s = 0; s < nStates; ++s)
        for (int a = 0; a < nActions; ++a)
        {
            traces[s][a] = 0;
        }
}


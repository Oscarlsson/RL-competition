#include "Agent.hpp"

#include <math.h>
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

int randInRange(int max);
double runif()
{
    return ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
}

Agent::Agent(int nStates, int nActions, double gamma, double lambda,
             double stepSize, double epsilon)
: nStates(nStates), nActions(nActions), gamma(gamma), lambda(lambda),
  stepSize(stepSize), t(0), e(0), policy(epsilon) /* EpsilonGreedyPolicy(epsilon) */
{
    srand(time(0));
    qTable = new double*[nStates];
    traces = new double*[nStates];
    for (int s = 0; s < nStates; ++s)
    {
        qTable[s] = new double[nActions];
        traces[s] = new double[nActions];
    }
    cout << "Initializing agent with parameters:" << endl
              << "\tnStates : "  << nStates  << endl
              << "\tnActions : " << nActions << endl
              << "\tgamma : "    << gamma    << endl
              << "\tlambda : "   << lambda   << endl
              << "\tepsilon : "   << epsilon   << endl;
}

Agent::~Agent()
{
    std::cout << "~Agent()" << endl;
    for (int i = 0; i < nStates; ++i)
    {
        delete [] qTable[i];
        delete [] traces[i];
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

    // Choose A2 from S2 using policy derived from Q (e.g. epsilon-greedy)
    int A2 = policy.sample_action(S2, t, qTable, nActions);

    double delta = reward + gamma * qTable[S2][A2] - qTable[S][A];
    traces[S][A] += 1;

    for (int ti = 0; ti <= t; ++ti)
    {
        int s = history_S[ti];
        int a = history_A[ti];
        qTable[s][a] += stepSize * delta * traces[s][a];
        traces[s][a] = gamma * lambda * traces[s][a];
    }

    ++t;
    return A2;
}

void Agent::start()
{
    t = 0;
    ++e;
    cout << "\t" << e << "\r";
    cout.flush();
    for (int s = 0; s < nStates; ++s)
        for (int a = 0; a < nActions; ++a)
        {
            traces[s][a] = 0;
        }
}

EpsilonGreedyPolicy::EpsilonGreedyPolicy(double epsilon)
    : epsilon(epsilon)
{}

int EpsilonGreedyPolicy::sample_action(int S, int t, double **qTable,
                                       int nActions)
{
    // Ignore time, but could use epsilon/t.
    if (runif() < epsilon)
        return randInRange(nActions-1);

    int aMax;
    double qMax = -DBL_MAX;
    for (int a = 0; a < nActions; ++a)
    {
        if (qTable[S][a] > qMax)
        {
            qMax = qTable[S][a];
            aMax = a;
        }
    }

    return aMax;
}


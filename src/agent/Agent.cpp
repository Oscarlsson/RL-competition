#include "Agent.hpp"

// #include <math.h>
#include <cmath>
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
    }


    for (int s = 0; s < nStates; ++s)
        for (int a = 0; a<nActions; ++a)
        {
            qTable[s][a] = maxReward;
            counts[s][a] = 1;
        }

    cerr << "Initializing agent with parameters:" << endl
              << "\tnStates : "  << this->nStates  << endl
              << "\tnActions : " << this->nActions << endl
              << "\tgamma : "    << this->gamma    << endl
              << "\tlambda : "   << this->lambda   << endl
              << "\tstepsize : " << this->stepSize << endl;
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

    // Choose A2 from S2 using policy derived from Q
    // UCB1
    int A2 = policy.sample_action(S2, t, qTable, counts, nActions, history_S,
                                   lambda);
    

    double delta = reward + gamma * qTable[S2][A2] - qTable[S][A];
    traces[S][A] += 1;

    for (int s = 0; s < nStates; ++s)
    {
        for (int a = 0; a < nActions; ++a)
        {
            qTable[s][a] += stepSize * delta * traces[s][a];
            counts[s][a] += traces[s][a]; 
            traces[s][a]  = gamma * lambda * traces[s][a];
        }
    }

// Test this?
//  A2 = policy.sample_action(S2, t, qTable, counts, nActions, history_S,
//                                 lambda);

    ++t;
    return A2;
}

int UCB1Policy::sample_action(int S, int t, double **qTable, double **counts,
                              int nActions, vector<int> &history_S,
                              double lambda)
{
    int aMax;
    double uMax = -DBL_MAX;
    double oMax = -DBL_MAX;
    bool found = false;
    double localTime = 0;
    for (int a = 0; a < nActions; ++a)
        localTime += counts[S][a];
    for (int a = 0; a < nActions; ++a)
    {
        double o = tieBreakerScore(a, S, t, qTable, counts, nActions,
                history_S, lambda);
        double u = qTable[S][a] + sqrt(2 * log(localTime+1) / counts[S][a]);
        if (u > uMax)
        {
            uMax = u;
            aMax = a;
            oMax = o;
            found = true;
        }
        else if (u == uMax && true)
        {
            if (o > oMax)
            {
                // cerr << "R";
                oMax = o;
                aMax = a;
            }
        }
    }
    if (!found)
    {
        cerr << "counts: ";
        for (int a = 0; a < nActions; ++a)
            cerr << counts[S][a] << "\t";
        cerr << endl << "qTable: ";
        for (int a = 0; a < nActions; ++a)
            cerr << qTable[S][a] << "\t";
        cerr << endl << "u:      ";
        for (int a = 0; a < nActions; ++a)
        {
            double u = qTable[S][a] + sqrt(2 * log((t+1) / counts[S][a]));
            cerr << qTable[S][a] << " + " << "sqrt(2 * log " << (t+1)
                << " / " << counts[S][a] << ") = ";
            cerr << u << "\t";
        }
        cerr << endl;
        cerr << "Action: " << aMax << endl;
    }
    return aMax;
}

double UCB1Policy::tieBreakerScore(int a, int S, int t, double **qTable,
                                   double **counts, int nActions,
                                   vector<int> &history_S, double lambda)
{
    double pow = 1;
    int si;
    double score = 0;
    
    double countNorm = 0;
    for (int hi = t; hi >= 0; --hi)
    {
        si = history_S[hi];
        score += pow * qTable[si][a]*sqrt(counts[si][a]);
        countNorm += sqrt(counts[si][a]);
        pow *= lambda; //Probably need HARDER PENALTY/state degradation, or remove cTable, between 1/9 (tree search)
    }
    score/=countNorm;
    
    return score;
}

void Agent::start()
{
    t = 0;
    ++e;
    cerr << "\t" << e << "\r";
    cerr.flush();
    for (int s = 0; s < nStates; ++s)
        for (int a = 0; a < nActions; ++a)
        {
            traces[s][a] = 0;
        }
}


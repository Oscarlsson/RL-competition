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

int randInRange(int max);
double runif()
{
    return ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
}

Agent::Agent(int nStates, int nActions, double gamma, double lambda,
             double stepSize, double epsilon, double minReward,
             double maxReward)
: nStates(nStates), nActions(nActions), gamma(gamma), lambda(lambda),
  stepSize(stepSize), t(0), e(0), policy(epsilon), /* EpsilonGreedyPolicy(epsilon) */
  minReward(minReward), maxReward(maxReward)
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
              << "\tnStates : "  << nStates  << endl
              << "\tnActions : " << nActions << endl
              << "\tgamma : "    << gamma    << endl
              << "\tlambda : "   << lambda   << endl
              << "\tstepsize : " << stepSize << endl
              << "\tepsilon : "  << epsilon  << endl;
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

    if (t < 0)
    {
        cerr << "r:" << reward << " t: " << t << ":: ";
        for (int ti = 0; ti < t; ++ti)
            cerr << history_A[ti] << " ";
        cerr << "\n";
        cerr.flush();
    }
    
    // Choose A2 from S2 using policy derived from Q (e.g. epsilon-greedy)
    // UCB1
    int A2 = policy2.sample_action(S2, t, qTable, counts, nActions);
    // cerr << "\tA':" << A2 << endl;
    // eGreedy
    // int A2 = policy.sample_action(S2, t, qTable, nActions);
    // "Tiebreaker"
    // int A2 = sample_action(S2, t, qTable, counts, nActions, lambda, gamma, history_S);

    //beta = updateCorrelationMatrices(reward, beta, history_A);
    
    double delta = reward + gamma * qTable[S2][A2] - qTable[S][A];
    traces[S][A] += 1;

    for (int s = 0; s < nStates; ++s)
    {
        for (int a = 0; a < nActions; ++a)
        {
            qTable[s][a] += stepSize * delta * traces[s][a];
            traces[s][a]  = gamma * lambda * traces[s][a];
            counts[s][a] += traces[s][a]; 
        }
    }

    ++t;
    return A2;
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

int UCB1Policy::sample_action(int S, int t, double **qTable, double **counts,
                              int nActions)
{
    int aMax;
    double uMax = -DBL_MAX;
    bool found = false;
    for (int a = 0; a < nActions; ++a)
    {
        double u = qTable[S][a] + sqrt(2 * log(t+1) / counts[S][a]);
        if (u > uMax)
        {
            uMax = u;
            aMax = a;
            found = true;
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

/* Just some random definition, not working very well, RETURNS FALSE UNTIL ALL ACTIONS TRIED ONCE, IS THIS SUPPOSED TO HAPPEN?*/
bool Agent::visited(int s)
{
    for (int a = 0; a < nActions; ++a)
        if (qTable[s][a] == 0)
            return false;
    return true;
}

int Agent::sample_action(int S, int t, double **qTable, double **counts, int nActions, double lambda,double gamma, vector<int> &history_S)
{
    if (visited(S))
        return policy2.sample_action(S, t, qTable, counts, nActions);

    double *actionProb = new double[nActions];

    for(int a= 0; a<nActions;++a)
        actionProb[a] = 0;
    
    double total = 0;
    double pow = 1;
    int si;
    
    for (int hi = t; hi >= 0; --hi)
    {
        si = history_S[hi];
        for (int a = 0; a < nActions; ++a)
        {
            actionProb[a] += pow * (qTable[si][a]- minReward)*counts[si][a];
            //minReward is not sufficient now, unless we start to use cummulative
            //reward inst of qTable, totalActionProb needs to converge for it to
            //be of any use
        }
        pow *= lambda*gamma; //Probably need HARDER PENALTY/state degradation, or remove cTable, between 1/9 (tree search)
    }
    
    for(int a=0;a<nActions;++a)
        total+=actionProb[a];
    /* DEBUG */
    /*if (t < 0)
    {
        for (int a = 0; a < nActions; ++a)
        {
            cerr << actionProb[a] / total << "\t";
        }
        cerr << "\n";
        cerr.flush();
    }*/

    double pSum = 0;
    double randomFraction = runif();
    for (int a = 0; a < nActions; ++a)
    {
        actionProb[a] /= total;
        pSum += actionProb[a];

        if (randomFraction <= pSum)
        {
            delete[] actionProb;
            return a;
        }
    }
}

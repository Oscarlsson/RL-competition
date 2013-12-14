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

    /*
     * "Tiebreaker"
     */
    rTable = new double*[nActions]; //Reward table
    cTable = new double*[nActions]; //Tally table
    for (int a = 0; a<nActions; ++a)
    {
        rTable[a] = new double[nActions];
        cTable[a] = new double[nActions];
        for (int a2 = 0; a2 < nActions; ++a2)
        {
            rTable[a][a2] = 0;
            cTable[a][a2] = 1;
        }
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
    for (int a = 0; a<nActions; ++a)
    {
        delete [] rTable[a];
        delete [] cTable[a];
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

    // updateCorrelationMatrices(reward, 0.99, history_A);

    // Choose A2 from S2 using policy derived from Q (e.g. epsilon-greedy)
    // UCB1
    int A2 = policy2.sample_action(S2, t, qTable, counts, nActions);
    // cerr << "\tA':" << A2 << endl;
    // eGreedy
    // int A2 = policy.sample_action(S2, t, qTable, nActions);
    // "Tiebreaker"
    // int A2 = sample_action(S2, t, qTable, nActions, 0.99, history_A);

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

/* Just some random definition, not working very well */
bool Agent::visited(int s)
{
    for (int a = 0; a < nActions; ++a)
        if (qTable[s][a] == 0)
            return false;
    return true;
}

int Agent::sample_action(int S, int t, double **qTable, int nActions, double beta, vector<int> &history_A)
{
    if (visited(S))
        return policy.sample_action(S, t, qTable, nActions);

    double *actionProb = new double[nActions];

    double total = 0;
    
    for (int a = 0; a < nActions; ++a)
    {
        actionProb[a] = 0;
        double pow = 1;
        for (int hi = t; hi >= 0; --hi)
        {
            int ai = history_A[hi];
            actionProb[a] += pow * (rTable[a][ai] / cTable[a][ai] - minReward);
            pow *= beta;
        }
        total += actionProb[a];
    }

    /* DEBUG */
    if (t < 0)
    {
        for (int a = 0; a < nActions; ++a)
        {
            cerr << actionProb[a] / total << "\t";
        }
        cerr << "\n";
        cerr.flush();
    }

    double pSum = 0;
    for (int a = 0; a < nActions; ++a)
    {
        actionProb[a] /= total;
        pSum += actionProb[a];

        if (runif() <= pSum)
        {
            delete[] actionProb;
            return a;
        }
    }
}

void Agent::updateCorrelationMatrices(double lastReward, double beta,
                                      vector<int> history_A)
    //gamma as a pointer, so it updates as well...
{
    for (int hi = t; hi > 0; --hi)
    {
        for (int hj = hi - 1; hj >= 0; --hj)
        {
            double discountFactor = pow(beta, t - hj - 1);
            double reward = lastReward * discountFactor;
            int ai = history_A[hi];
            int aj = history_A[hj];
            rTable[ai][aj] += reward;
            rTable[aj][ai] += reward;
            cTable[ai][aj] += discountFactor;
            cTable[aj][ai] += discountFactor;
        }
    }

    static int foo = 1;
    
    if (++foo < 0)
    {
        for (int a1 = 0; a1 < nActions; ++a1)
        {
            for (int a2 = 0; a2 < nActions; ++a2)
                cerr << rTable[a1][a2] << "/" << cTable[a1][a2] << "\t";
            cerr << endl;
        }
        cerr << endl;
    }

    // FIX GAMMA UPDATER WHEN REST WORKS
   /* double exp0[nActions];
    double exp1[nActions];
    
    exp0 = expectationFromCorrelations(* exp0, **rTable,
                                **cTable, gamma,
                                nActions, history_A,
                                               t-1);
    exp1 = expectationFromCorrelations(* exp1, **rTable,
                                               **cTable, gamma+0.001,
                                               nActions, history_A,
                                               t-1);
    gamma += (lastReward-exp0)/(exp1-exp0)*/ 
    
}

#include "UCB1Policy.hpp"

#include <vector>
#include <iostream>
#include <cfloat>
#include <cmath>

using namespace std;

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
        double o = tieBreakerScore(a, S, t, qTable, counts, nActions, history_S,
                                   lambda);
        double u = qTable[S][a] + sqrt(2 * log(localTime+1) / counts[S][a]);
        if (u > uMax)
        {
            uMax = u;
            aMax = a;
            oMax = o;
            found = true;
        }
        else if (u == uMax && false)
        {
            if (o > oMax)
            {
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


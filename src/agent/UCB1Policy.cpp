#include "UCB1Policy.hpp"

#include <vector>
#include <iostream>
#include <cfloat>
#include <cmath>
#include <cassert>

using namespace std;

int UCB1Policy::sample_action(int S, int t, double **qTable, double **counts,
        int nActions, vector<int> &history_S,
        double lambda, double Qmin, double Qmax)
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
        // double u = qTable[S][a] + 0.001 * sqrt(2 * log(localTime+1) / counts[S][a]);
        double u = newtonRapson((qTable[S][a]-Qmin)/(Qmax-Qmin), localTime, counts[S][a]);

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

double UCB1Policy::dfun(double p, double q)
{
    if (p == 0)
        return (1-p)*log((1-p)/(1-q));
    else if (p == 1)
        return p*log(p/q);
    else
        return p*log(p/q)+(1-p)*log((1-p)/(1-q));
}
double UCB1Policy::ddfun(double p, double q)
{
    return (1.0-p)/(1.0-q)-p/q;
}

double UCB1Policy::newtonRapson(double Q, double t,double count)
{
    if (Q == 1)
        Q = 0.999999;
    double q = (Q+1)/2;
    double qOld = 1;
    bool first = false;
    bool debug = false;
    if (debug) cerr << "*** Newton Raphson start : Q = " << Q << endl;
    while (first || abs(q-qOld) > 0.001)
    {
        first = false;
        if (debug) cerr << "\tqOld = " << qOld << "\tq= " << q << endl;
        qOld = q;
        if (debug) cerr << "\tdfun(Q,q) = " << dfun(Q,q) << "\tqqfun(Q,q) = " << ddfun(Q,q) << endl;
        // assert(!isinf(dfun(Q,q));
        assert(abs(dfun(Q,q)) > 0);
        double c = 0.0;
        q = (-dfun(Q,q)+c*log(t)/count+q*ddfun(Q,q))/(ddfun(Q,q));
        if (q<=Q)
            q = Q+0.001;
        if (q>=1)
            q = 0.999;
        if (debug) cerr << "\tqOld = " << qOld << "\tq= " << q << endl;
    }
    if (debug) cerr << "*** Newton Raphson end with q = " << q << endl;
    return q;
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
        pow *= 1/nActions; //Probably need HARDER PENALTY/state degradation, or remove cTable, between 1/9 (tree search)
    }
    score/=countNorm;
    
    return score;
}


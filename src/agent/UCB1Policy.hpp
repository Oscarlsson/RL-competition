#ifndef UCBPOLICY1_H
#define UCBPOLICY1_H

#include <vector>

class UCB1Policy
{
    public:
        double newtonRapson(double Q, double t,double count);
        double dfun(double p, double q);
        double ddfun(double p, double q);
        int sample_action(int S, int t, double **qTable, double **counts,
                          int nActions, std::vector<int> &history_S,
                          double lambda, double Qmin, double Qmax);
        double tieBreakerScore(int a, int S, int t, double **qTable,
                               double **counts, int nActions,
                               std::vector<int> &history_S, double lambda);
};

#endif

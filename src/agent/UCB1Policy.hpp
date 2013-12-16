#ifndef UCBPOLICY1_H
#define UCBPOLICY1_H

#include <vector>

class UCB1Policy
{
    public:
        double newtonRapson(double Q, double t,double count);
        int sample_action(int S, int t, double **qTable, double **counts,
                          int nActions, std::vector<int> &history_S,
                          double lambda);
        double tieBreakerScore(int a, int S, int t, double **qTable,
                               double **counts, int nActions,
                               std::vector<int> &history_S, double lambda);
};

#endif

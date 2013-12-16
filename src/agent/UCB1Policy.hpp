#ifndef UCBPOLICY1_H
#define UCBPOLICY1_H

#include <vector>
#include <stdlib.h>

class UCB1Policy
{
    protected:
    public:
        double c;
        bool tiebreaker;
        double newtonRapson(double Q, double t,double count);
        double dfun(double p, double q);
        double ddfun(double p, double q);
        int sample_action(int S, int t, double **qTable, double **counts,
                          int nActions, std::vector<int> &history_S,
                          double lambda, double Qmin, double Qmax);
        double tieBreakerScore(int a, int S, int t, double **qTable,
                               double **counts, int nActions,
                               std::vector<int> &history_S, double lambda);
        UCB1Policy()
        {
            char* env_c = getenv("LIBRLAGENT_C");
            c = env_c == NULL ?     0.0 : atof(env_c);

            char* env_tiebreaker = getenv("LIBRLAGENT_TIEBREAKER");
            tiebreaker = env_tiebreaker == NULL ? false : atoi(env_tiebreaker);
        }
};

#endif

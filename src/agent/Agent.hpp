#include <vector>

struct UCB1Policy
{
    int sample_action(int S, int t, double **qTable, double **counts,
                      int nActions, std::vector<int> &history_S, double lambda);
    double tieBreakerScore(int a, int S, int t, double **qTable,
                           double **counts, int nActions,
                           std::vector<int> &history_S, double lambda);
};

class Agent
{
    protected:
        double **qTable; // Q(s,a) = value of action a in state s
        double **traces; // Z(s,a) = eligibility trace for (s,a)
        double **counts; // 

        int t; // Time step
        int e; // Episode
        
        UCB1Policy policy;

    public:
        const int nStates;
        const int nActions;
        const double minReward, maxReward;
        const double gamma;
        const double lambda;   // May not be const?
        const double stepSize; // May not be const?

        Agent(int nStates, int nActions, double gamma, double lambda,
              double stepSize, double minReward, double maxReward);
        ~Agent();
        int step(int lastState, int lastAction, double reward, int thisState);
        void start();
};

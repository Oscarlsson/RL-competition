#include <vector>
/*
 * This may change ALOT, in the following ways:
 *  separate class,
 *  some abstract interface,
 *  new methods/structure if needed.
 *
 */
struct EpsilonGreedyPolicy
{
    double epsilon;

    EpsilonGreedyPolicy(double epsilon);

    int sample_action(int S, int t, double **qTable, int nActions);
};

struct UCB1Policy
{
    int sample_action(int S, int t, double **qTable, double **counts,
                      int nActions);
};

class Agent
{
    protected:
        double **qTable; // Q(s,a) = value of action a in state s
        double **traces; // Z(s,a) = eligibility trace for (s,a)
        double **counts; // 

        int t; // Time step
        int e; // Episode
        
        EpsilonGreedyPolicy policy; // Would be nicer as an abstract class
        UCB1Policy policy2;

        int sample_action(int S, int t, double **qTable, int nActions,
                          double lambda, double gamma, std::vector<int> &history_S);
        //double* expectationFromCorrelations(double * actionExpt, double beta,
        //                                   std::vector<int> &history_A);
        //double updateCorrelationMatrices(double lastReward, double beta,
        //                                   std::vector<int> &history_A);
        bool visited(int s);

    public:
        const int nStates;
        const int nActions;
        const double minReward, maxReward;
        const double gamma;
        const double lambda;   // May not be const?
        const double stepSize; // May not be const?

        Agent(int nStates, int nActions, double gamma, double lambda,
              double stepSize, double epsilon, double minReward,
              double maxReward);
        ~Agent();
        int step(int lastState, int lastAction, double reward, int thisState);
        void start();
};

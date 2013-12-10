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

class Agent
{
    protected:
        double **qTable; // Q(s,a) = value of action a in state s
        double **traces; // Z(s,a) = eligibility trace for (s,a)

        int t;

        EpsilonGreedyPolicy policy; // Would be nicer as an abstract class

    public:
        const int nStates;
        const int nActions;
        const double gamma;
        const double lambda;   // May not be const?
        const double stepSize; // May not be const?

        Agent(int nStates, int nActions, double gamma, double lambda,
              double stepSize, double epsilon);
        ~Agent();
        int step(int lastState, int lastAction, double reward, int thisState);
        void start();
};

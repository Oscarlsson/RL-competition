#include <vector>
#include "UCBPolicy.hpp"

class Agent
{
    protected:
        double **qTable; // Q(s,a) = value of action a in state s
        double **traces; // Z(s,a) = eligibility trace for (s,a)
        double **counts; // 


    public:
        UCBPolicy policy;
        int t; // Time step
        int tSum; 
        int e; // Episode
        double cumulativeReward;
        const int nStates;
        const int nActions;
        const double minReward, maxReward;
        double Qmin, Qmax; //constant?
        const double gamma;
        const double lambda;   // May not be const?
        const double stepSize; // May not be const?

        Agent(int nStates, int nActions, double gamma, double lambda,
              double stepSize, double minReward, double maxReward);
        ~Agent();
        void printValueFunctionForMines();
        int step(int lastState, int lastAction, double reward, int thisState);
        void start();
};

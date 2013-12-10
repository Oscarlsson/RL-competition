class Agent
{
    protected:
        double **qTable; // Q(s,a) = value of action a in state s
        double **traces; // Z(s,a) = eligibility trace for (s,a)

    public:
        const int nStates;
        const int nActions;
        const double gamma;
        const double lambda; // May not be const?

        Agent(int nStates, int nActions, double gamma, double lambda);
        int step(double reward, int newState);
};

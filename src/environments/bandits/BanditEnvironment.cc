/**
   Copyright (C) 2013, Christos Dimitrakakis
*/

#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>      

// env_ function prototypes types 
#include <rlglue/Environment_common.h>	  

// helpful functions for allocating structs and cleaning them up 
#include <rlglue/utils/C/RLStruct_util.h> 
double runif()
{
    return ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
}
class BernoulliBandits
{
public:
	std::vector<double> means;
	//std::default_random_engine generator;
	observation_t observation;
	reward_observation_terminal_t reward_observation;
	int n_bandits;
	double discount;
	BernoulliBandits(int n, double discount_)
		: means(n),
		  n_bandits(n),
		  discount(discount_)
	{
        srand(time(0));
		std::cout << "# Creating Bernoulli bandits with "
				  << n << " arms, "
				  << discount << " discounting."
				  << std::endl;

		for (int i=0; i<n; ++i) {
			means[i] = runif();
		}
		allocateRLStruct(&observation, 1, 0, 0);
		reward_observation.observation = &observation;
		reward_observation.reward = 0;
		reward_observation.terminal = 0;
	}
	virtual ~BernoulliBandits()
	{
		clearRLStruct(&observation);
	}

	virtual std::string task_spec_string() 
	{
		std::ostringstream str;

		str << "VERSION RL-Glue-3.0 PROBLEMTYPE episodic"
			<< " DISCOUNTFACTOR " << discount 
			<< " OBSERVATIONS INTS (0 0)" 
			<< " ACTIONS INTS (0 " << n_bandits - 1 << ")" 
			<< " REWARDS (0.0 1.0)"
			<< " EXTRA Bernoulli Bandits (C/C++) by Christos Dimitrakakis";
		
		return str.str();
	}

	
	const reward_observation_terminal_t* Step(const action_t *action)
	{
		int action_value = action->intArray[0];
		double reward = GenerateReward(action_value);
		reward_observation.observation->intArray[0] = 0;
		reward_observation.reward = reward;
		if (runif() >= discount) {
			reward_observation.terminal = true;
		} else {
			reward_observation.terminal = false;
		}
		//std::cout << action_value << " " << reward << std::endl;
		return &reward_observation;
	}
	
	const observation_t* Start()
	{
		observation.intArray[0] = 0;
		reward_observation.terminal = false;
		return &observation;
	}

	double GenerateReward(int a)
	{
        return (runif() <= means[a]);
	}
};

// HELPER FUNCTIONS.  PROTOTYPES HERE, CODE AT THE BOTTOM OF FILE */

/* GLOBAL VARIABLES FOR RL-GLUE methods (global for convenience) */  

static BernoulliBandits* environment;


/** Initialise environment at the beginning of a run.
*/
const char* env_init(){    
    
    int n_arms = (int)(runif()*126+2);
	double discount = 0.99;
	environment = new BernoulliBandits(n_arms, discount);
	return environment->task_spec_string().c_str(); 
}


/**
   Standard RL-Glue method. Sets an initial state and returns
   the corresponding observation.
*/
const observation_t *env_start()
{ 
	return environment->Start();
}

const reward_observation_terminal_t *env_step(const action_t *action)
{
	/* Make sure the action is valid */
	return environment->Step(action);
}

/** Clean-up the environment at the end of a run */
void env_cleanup()
{
	delete environment;
}

/** Send a message to the environment */
const char* env_message(const char* _inMessage) {
  
	std::string inMessage = _inMessage;

	if(inMessage == "test") {
        return "Test!";
    }
    
	return "bandit environment (C++) does not respond to that message.";
}


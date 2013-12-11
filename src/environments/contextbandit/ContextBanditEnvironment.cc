/**
   Copyright (C) 2013, Christos Dimitrakakis
*/

#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <sstream>      

// env_ function prototypes types 
#include <rlglue/Environment_common.h>	  

// helpful functions for allocating structs and cleaning them up 
#include <rlglue/utils/C/RLStruct_util.h> 

static std::default_random_engine generator;


class ContextBandits
{
public:
	std::vector< std::vector<double> > means;
	//std::default_random_engine generator;
	std::uniform_real_distribution<double> uniform_distribution;
	std::uniform_int_distribution<int> context_distribution;
	observation_t observation;
	reward_observation_terminal_t reward_observation;
	int n_bandits;
	int n_contexts;
	double discount;
	int state;
	ContextBandits(int m, int n, double discount_)
		: means(m),
		  uniform_distribution(0.0, 1.0),
		  context_distribution(0, m - 1),
		  n_bandits(n),
		  n_contexts(m),
		  discount(discount_),
		  state(0)
	{
		std::cout << "# Creating Context bandits with "
				  << n_bandits << " arms, "
				  << n_contexts << " contexts "
				  << discount << " discounting."
				  << std::endl;
		
		for (int i=0; i<n_contexts; ++i) {
			means[i].resize(n_bandits);
			for (int j=0; j<n_bandits; ++j) {
				means[i][j] = uniform_distribution(generator);
			}
		}
		allocateRLStruct(&observation, 1, 0, 0);
		reward_observation.observation = &observation;
		reward_observation.reward = 0;
		reward_observation.terminal = 0;
	}
	virtual ~ContextBandits()
	{
		clearRLStruct(&observation);
	}

	virtual std::string task_spec_string() 
	{
		std::ostringstream str;

		str << "VERSION RL-Glue-3.0 PROBLEMTYPE episodic"
			<< " DISCOUNTFACTOR " << discount 
			<< " OBSERVATIONS INTS (0 " << n_contexts - 1 << ")" 
			<< " ACTIONS INTS (0 " << n_bandits - 1 << ")" 
			<< " REWARDS (0.0 1.0)"
			<< " EXTRA Context Bandit Training Environment Christos Dimitrakakis";
		
		return str.str();
	}

	
	const reward_observation_terminal_t* Step(const action_t *action)
	{
		int action_value = action->intArray[0];
		double reward = GenerateReward(action_value);
		reward_observation.observation->intArray[0] = 0;
		reward_observation.reward = reward;
		if (uniform_distribution(generator) >= discount) {
			reward_observation.terminal = true;
		} else {
			reward_observation.terminal = false;
		}
		//std::cout << action_value << " " << reward << std::endl;
		return &reward_observation;
	}
	
	const observation_t* Start()
	{
		state = context_distribution(generator);
		observation.intArray[0] = state;
		reward_observation.terminal = false;
		return &observation;
	}

	double GenerateReward(int a)
	{
		std::bernoulli_distribution distribution(means[state][a]);
		return distribution(generator);
	}
};

// HELPER FUNCTIONS.  PROTOTYPES HERE, CODE AT THE BOTTOM OF FILE */

/* GLOBAL VARIABLES FOR RL-GLUE methods (global for convenience) */  

static ContextBandits* environment;


/** Initialise environment at the beginning of a run.
*/
const char* env_init(){    
	std::uniform_int_distribution<int> arms_distribution(2,128);
	int n_arms = arms_distribution(generator);
	std::uniform_int_distribution<int> context_distribution(1,16);
	int n_contexts = context_distribution(generator);
	double discount = 0.99;
	environment = new ContextBandits(n_contexts, n_arms, discount);
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


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

static std::default_random_engine generator((unsigned int)time(0));


class LoopEnvironment
{
public:
    std::uniform_real_distribution<double> uniform_distribution;
    observation_t observation;
    reward_observation_terminal_t reward_observation;
    int medium_state;
    int good_state;
    int n_states;
    int state;
    double discount;
    double flip_probability;
    LoopEnvironment(int n, double discount_, double flip=0.2)
		: 	 uniform_distribution(0.0, 1.0),
			 medium_state(n),
			 good_state(2 * n),
			 n_states(2 * n + 1),
			 state(0),
			 discount(discount_),
			 flip_probability(flip)
    {
		std::cout << "# Creating Loop environment with "
				  << n_states << " states, "
				  << discount << " discounting."
				  << std::endl;
		allocateRLStruct(&observation, 1, 0, 0);
		reward_observation.observation = &observation;
		reward_observation.reward = 0;
		reward_observation.terminal = 0;
    }
    virtual ~LoopEnvironment()
    {
		clearRLStruct(&observation);
    }

    virtual std::string task_spec_string() 
    {
		std::ostringstream str;

		str << "VERSION RL-Glue-3.0 PROBLEMTYPE episodic"
			<< " DISCOUNTFACTOR " << discount 
			<< " OBSERVATIONS INTS (0 "  << n_states - 1 << ")" 
			<< " ACTIONS INTS (0 1)" 
			<< " REWARDS (0.0 1.0)"
			<< " EXTRA Loop environment (C/C++) by Christos Dimitrakakis";
		return str.str();
    }

    const reward_observation_terminal_t* Step(const action_t *action)
    {
		int action_value = action->intArray[0];
		if (uniform_distribution(generator) <= flip_probability) {
			action_value = 1 - action_value;
		}
		double reward = 0;
		if (state == good_state) {
			reward = 2.0;
		} else if (state == medium_state) {
			reward = 1.0;
		}
		if (state == good_state || state == medium_state) {
			state = 0;
		} else if (state == 0) {
			if (action == 0) {
				state = 1;
			} else {
				state = medium_state + 1;
			} 
		} else if (state < medium_state) {
			state ++;
		} else  if (state < good_state && state > medium_state) {
			if (action == 0) {
				state = 0;
			} else {
				state++;
			}
		} else {
			printf("# warning: stuck in state %d\n", state);
		}

		reward_observation.observation->intArray[0] = state;
		reward_observation.reward = reward;
		if (uniform_distribution(generator) >= discount) {
			reward_observation.terminal = true;
		} else {
			reward_observation.terminal = false;
		}
		return &reward_observation;
    }
	
    const observation_t* Start()
    {
		observation.intArray[0] = 0;
		reward_observation.terminal = false;
		return &observation;
    }


};

// HELPER FUNCTIONS.  PROTOTYPES HERE, CODE AT THE BOTTOM OF FILE */

/* GLOBAL VARIABLES FOR RL-GLUE methods (global for convenience) */  

static LoopEnvironment* environment;


/** Initialise environment at the beginning of a run.
 */
const char* env_init(){    
    //std::uniform_int_distribution<int> states_distribution(4, 8);
    int ring_size = 4; //states_distribution(generator);
    double discount = 0.99;
	double flip = 0.2;
    environment = new LoopEnvironment(ring_size, discount, flip);
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


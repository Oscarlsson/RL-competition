#include <iostream>
#include <stdlib.h>
#include <string.h> /* for strcmp */
#include <time.h> /*for time()*/
#include <rlglue/Agent_common.h> /* agent_ function prototypes and RL-Glue types */
#include <rlglue/utils/C/RLStruct_util.h> /* helpful functions for allocating structs and cleaning them up */
#include <rlglue/utils/C/TaskSpec_Parser.h>
#include "Agent.hpp"

using namespace std;

action_t this_action;
action_t last_action;

Agent *agent;

observation_t *last_observation=0;

void agent_init(const char* task_spec)
{
	// Parse task specifications
	taskspec_t* ts = (taskspec_t*)malloc(sizeof(taskspec_t));
	int decode_result = decode_taskspec( ts, task_spec );
	if (decode_result!=0)
    {
        cout << "Could not decode task spec, code: "
             << decode_result << "for task spec: " << task_spec << endl;
		exit(1);
	}
    if (ts->num_int_observations != 1)
    {
        cout << "Agent only works with 1 observation." << endl;
		exit(1);
    }

    char* env_lambda = getenv("LIBRLAGENT_LAMBDA");
    char* env_stepsize = getenv("LIBRLAGENT_STEPSIZE");

    double lambda = env_lambda == NULL ?     0.20 : atof(env_lambda);
    double stepsize = env_stepsize == NULL ? 1.00 : atof(env_stepsize);

    // Assumes that all actions and states are numbered 0,1,2,3,...
    // I.e. agent breaks if negative indices exist or if indexing skips some int
    int nActions = ts->int_actions[0].max - ts->int_actions[0].min + 1;
    int nStates = ts->int_observations[0].max - ts->int_observations[0].min + 1;
    agent = new Agent(
                ts->int_observations[0].max - ts->int_observations[0].min + 1,
                nActions,
                ts->discount_factor, // Gamma
                lambda, stepsize,
                ts->reward.min, ts->reward.max
            );
    
	srand(time(0));
	allocateRLStruct(&this_action,1,0,0);
	last_observation=allocateRLStructPointer(0,0,0);
	/* That is equivalent to:
			 this_action.numInts     =  1;
			 this_action.intArray    = (int*)calloc(1,sizeof(int));
			 this_action.numDoubles  = 0;
			 this_action.doubleArray = 0;
			 this_action.numChars    = 0;
			 this_action.charArray   = 0;
	*/
}


const action_t *agent_start(const observation_t *this_observation)
{
    agent->start();

    this_action.intArray[0] =
        agent->step(0,                              // Last state
                    0,                              // Last action
                    0,                              // Reward
                    this_observation->intArray[0]); // This state

	replaceRLStruct(&this_action, &last_action);
	replaceRLStruct(this_observation, last_observation);

    return &this_action;
}

const action_t *agent_step(double reward, const observation_t *this_observation)
{
    this_action.intArray[0] =
        agent->step(last_observation->intArray[0],  // Last state
                    last_action.intArray[0],       // Last action
                    reward,                         // Reward
                    this_observation->intArray[0]); // This state

	replaceRLStruct(&this_action, &last_action);
	replaceRLStruct(this_observation, last_observation);

    return &this_action;
}

void agent_end(double reward)
{
    agent->step(last_observation->intArray[0],  // Last state
                last_action.intArray[0],        // Last action
                reward,                         // Reward
                last_observation->intArray[0]); // ...
	clearRLStruct(&last_action);
	clearRLStruct(last_observation);
}

void agent_cleanup()
{
    cout << "agent_cleanup() called" << endl;
	clearRLStruct(&this_action);
	clearRLStruct(&last_action);
	freeRLStructPointer(last_observation);
}

const char* agent_message(const char* inMessage)
{
	if(strcmp(inMessage,"what is your name?")==0)
		return "my name is skeleton_agent!";
	return "I don't know how to respond to your message";
}

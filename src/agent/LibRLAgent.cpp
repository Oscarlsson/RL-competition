#include <stdio.h>  /* for printf */
#include <string.h> /* for strcmp */
#include <time.h> /*for time()*/
#include <rlglue/Agent_common.h> /* agent_ function prototypes and RL-Glue types */
#include <rlglue/utils/C/RLStruct_util.h> /* helpful functions for allocating structs and cleaning them up */

action_t this_action;
action_t last_action;

observation_t *last_observation=0;

void agent_init(const char* task_spec)
{
    /* Here is where you might allocate storage for parameters (value function or
     * policy, last action, last observation, etc) */
	/*Here you would parse the task spec if you felt like it */
    /* Allocate memory for a one-dimensional integer action using utility
     * functions from RLStruct_util */
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

const action_t *tempAct(const observation_t *this_observation)
{
	this_action.intArray[0] = 1; // Always action 1
    // Store last observations
	replaceRLStruct(&this_action, &last_action);
	replaceRLStruct(this_observation, last_observation);
	
	return &this_action;
}

const action_t *agent_start(const observation_t *this_observation)
{
    return tempAct(this_observation);
}

const action_t *agent_step(double reward, const observation_t *this_observation)
{
    return tempAct(this_observation);
}

void agent_end(double reward)
{
	clearRLStruct(&last_action);
	clearRLStruct(last_observation);
}

void agent_cleanup()
{
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

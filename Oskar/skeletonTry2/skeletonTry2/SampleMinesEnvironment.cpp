/* 
	Copyright (C) 2008, Brian Tanner

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	    http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

	This code is adapted from the Mines.cpp code written by Adam White
	for earlier versions of RL-Glue.
	
	*  $Revision: 996 $
	*  $Date: 2009-02-08 20:48:32 -0500 (Sun, 08 Feb 2009) $
	*  $Author: brian@tannerpages.com $
	*  $HeadURL: https://rl-library.googlecode.com/svn/trunk/projects/packages/examples/mines-sarsa-c/SampleMinesEnvironment.c $
	
*/

#include <stdio.h>   /*sprintf*/

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>

// env_ function prototypes types 
#include <rlglue/Environment_common.h>	  

// helpful functions for allocating structs and cleaning them up 
#include <rlglue/utils/C/RLStruct_util.h> 

using namespace std; 

/* 	
	This is a very simple discrete-state, episodic grid world that has 
	exploding mines in it.  If the agent steps on a mine, the episode
	ends with a large negative reward.
	
	The reward per step is -1, with +10 for exiting the game successfully
	and -100 for stepping on a mine.
*/

/*
	world_description_t is a structure that holds all of the information
	about the world, including the placement of the mines and the position
	of the agent, etc.
*/

// HELPER FUNCTIONS.  PROTOTYPES HERE, CODE AT THE BOTTOM OF FILE */

int is_win(int row, int col);

int is_illegal_move(int row, int col);

int ai_random_move_is_win();

int flatten_state();

int is_legal_moves_left();

void print_state();

/*
	world_map is an array that describes the world.

	To read this: the world is a 6 by 18 grid in any position the number 
	corresponds to one of {START, GOAL, FREE, OBSTACLE, MINE}

	For example in env_init the start position is labelled by a 0
	so we can see the initial start position in this particular map is at
	position [12][1]
	
*/

/* GLOBAL VARIABLES FOR RL-GLUE methods (global for convenience) */  
static observation_t this_observation;
static reward_observation_terminal_t this_reward_observation;

int current_state=0;
int fixed_start_state = 1;
int multiplier[9] = {1,3, 9, 27, 81, 243, 729, 2187, 6561};
int current_board[3][3] =
{
    {0,0,0},
    {0,0,0},
    {0,0,0}
};

/* Used if a message is sent to the environment to use fixed start states */

static string task_spec_string =  
"VERSION RL-Glue-3.0 PROBLEMTYPE episodic \
DISCOUNTFACTOR 1 OBSERVATIONS INTS (0 19682) \
ACTIONS INTS (0 8)  REWARDS (-1. 1.0) \
EXTRA tictactoe_environment(C/C++) by Oskar Lindgren.";

/*****************************

	RL-Glue Methods 
	
*******************************/

const char* env_init(){
    
	/* Allocate the observation variable */
	allocateRLStruct(&this_observation,1,0,0);
	/* That is equivalent to:
		 this_observation.numInts     =  1;
		 this_observation.intArray    = (int*)calloc(1,sizeof(int));
		 this_observation.numDoubles  = 0;
		 this_observation.doubleArray = 0;
		 this_observation.numChars    = 0;
		 this_observation.charArray   = 0;
	*/
	/* Setup the reward_observation variable */
	this_reward_observation.observation=&this_observation;
	this_reward_observation.reward=0;
	this_reward_observation.terminal=0;

  return task_spec_string.c_str(); 
}

int ai_random_move_is_win(){
	int row=rand()%3;
	int col=rand()%3;
    
	while(is_illegal_move(row,col)){
		row=rand()%3;
		col=rand()%3;
	}
    
    current_board[row][col] = 2;
    if(is_win(row,col)){
        return 1;
    }
    return 0;
}

int flatten_state(){
    current_state = 0;
    int counter = 0;
    for(int i = 0; i<3;i++){
        for(int j = 0; j<3;j++){
            current_state+=multiplier[counter]*current_board[i][j];
            counter++;
        }
    }
    return current_state;
}
/*
	Standard RL-Glue method. Sets an initial state and returns
	the corresponding observation.
*/
const observation_t *env_start()
{
    
    for (int i = 0; i<3; i++) {
        for (int j=0; j<3; j++) {
            current_board[i][j] = 0;
        }
    }
    current_state=0;
	this_observation.intArray[0]=current_state;
  	return &this_observation;
}

int is_legal_moves_left(){
    for(int j = 0;j<3;j++){
        for(int i= 0; i<3; i++){
            if(!is_illegal_move(i,j)){
                return 1;
            }
        }
    }
    print_state();
    cout<<"No legal moves left"<<"\n";
    return 0;
}

const reward_observation_terminal_t *env_step(const action_t *this_action)
{
	/* Make sure the action is valid */
	assert(this_action->numInts==1);
	assert(this_action->intArray[0]>=0);
	assert(this_action->intArray[0]<9);
    
    int row = (this_action->intArray[0])/3;
	int col = (this_action->intArray[0])%3;

    int episode_over = 0;
    double the_reward=0;
    
    if(is_illegal_move(row,col)){
		episode_over=1;
		the_reward=-1;
    }else{
        
        current_board[row][col] = 1;
        
        if(is_win(row,col) || !is_legal_moves_left()){
            episode_over=1;
            the_reward=1;
        }else{
            if(ai_random_move_is_win()){
                current_state=0;
                episode_over=1;
                the_reward=-1;
            }
        }
    }
    //if(episode_over){cout<<"oneMoreFail\n";}
  
	this_reward_observation.observation->intArray[0] = flatten_state();
	this_reward_observation.reward = the_reward;
	this_reward_observation.terminal = episode_over;

	return &this_reward_observation;
}

void env_cleanup()
{
	clearRLStruct(&this_observation);
}

const char* env_message(const char* _inMessage) {
	/*	Message Description
 	 * 'set-random-start-state'
	 * Action: Set flag to do random starting states (the default)
	 */
  
  string inMessage = _inMessage;

	if(inMessage == "set-random-start-state") {
        fixed_start_state=0;
        return "Message understood.  Using random start state.";
    }
	
	/*	Message Description
 	 * 'print-state'
	 * Action: Print the map and the current agent location
	 */
	if(inMessage == "print-state"){
		print_state();
		return "Message understood.  Printed the state.";
  }

	return "SamplesMinesEnvironment(C++) does not respond to that message.";
}


/*****************************

	Helper Methods 
	
*******************************/

int is_win(int row, int col){
	int winwin = 0;
    int win=1;
	for (int i = 0; i<3 ; i++){
        if(current_board[row][i] != current_board[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1;}
    
    win=1;
    for (int i = 0; i<3 ; i++){
        if(current_board[i][col] != current_board[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1;}
    
    win=1;
    for (int i = 0; i<3 ; i++){
        if(current_board[i][i] != current_board[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1;}
    
    win=1;
    for (int i = 0; i<3 ; i++){
        if(current_board[i][2-i] != current_board[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1;}
    
    if(winwin){
        print_state();
        cout<<"Last move: row: "<<row<<", col: "<<col<<"\n"; 
    }

	return winwin;
}

int is_illegal_move(int row, int col){
	if (current_board[row][col] != 0){
		return 1;
        print_state();
        cout<<"Attempted move: row: "<<row<<", col: "<<col<<"\n";
        
	}
	return 0;
}

void print_state(){
    int row,col;
 
    for(row=0;row<3;row++){
        cout <<"\n";
        for(col=0;col<3;col++){
            cout <<current_board[row][col]<<" ";
        }
    }
    cout << "\n";
}

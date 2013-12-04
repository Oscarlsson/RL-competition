/*
 * Copyright (C) 2008, Brian Tanner
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include <string.h> /*strcmp*/
#include <stdio.h> /*printf*/
#include <stdlib.h>

#include <rlglue/Environment_common.h>/* env_ function prototypes and RL-Glue types */
#include <rlglue/utils/C/RLStruct_util.h> /* helpful functions for allocating structs and cleaning them up */

/* This is a very simple environment with discrete observations corresponding to states labeled {0,1,...,19,20}
 The starting state is 10.
 
 There are 2 actions = {0,1}.  0 decrements the state, 1 increments the state.
 
 The problem is episodic, ending when state 0 or 20 is reached, giving reward -1 or +1, respectively.  The reward is 0 on
all other steps.
*/

observation_t this_observation;
reward_observation_terminal_t this_reward_observation;
int current_state=0;
int current_board[3][3] =
{
	{0,0,0},
	{0,0,0},
	{0,0,0}
};
int multiplier[9]={1, 3, 9, 27, 81, 243, 729, 2187, 6561};


const char* env_init()
{
	char* task_spec="VERSION RL-Glue-3.0 PROBLEMTYPE episodic DISCOUNTFACTOR 1.0 OBSERVATIONS INTS (0 19682) ACTIONS INTS (0 8)  REWARDS (-1.0 1.0)  EXTRA tictactoe_environment(C/C++) by Oskar Lindgren.";
    
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
    
    return task_spec;
}

int check_illega_move(int row, int col){
	if (current_board[row][col] != 0){
		return 1;
	}
	return 0;
}

int check_win(int row, int col){
	int winwin = 0;
    int win=1;
	for (int i = 0; i<3 ; i++){
        if(current_board[row][i] != current_board[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1};
    
    win=1;
    for (int i = 0; i<3 ; i++){
        if(current_board[i][col] != current_board[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1};
    
    win=1;
    for (int i = 0; i<3 ; i++){
        if(current_board[i][i] != current_board[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1};
    
    win=1;
    for (int i = 0; i<3 ; i++){
        if(current_board[i][2-i] != current_board[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1};
    
	return winwin;
}

const observation_t *env_start()
{
	current_state=0;
	this_observation.intArray[0]=current_state;
  	return &this_observation;
}

const reward_observation_terminal_t *env_step(const action_t *this_action)
{
	int row = (this_action->intArray[0])/3;
	int col = (this_action->intArray[0])%3;
    
    int episode_over = 0;
    double the_reward=0;
    
    if(check_illega_move(row,col)){
        current_state=0;
		episode_over=1;
		the_reward=-1;
    }else{
        current_state += multiplier[this_action->intArray[0]];
        current_board[col][row] = 1;
        
        if(check_win(row,col)){
            current_state=0;
            episode_over=1;
            the_reward=1;
        }else{
        
        
        
        
        
        
        }
    }
    
    
    
	this_reward_observation.observation->intArray[0] = current_state;
	this_reward_observation.reward = the_reward;
	this_reward_observation.terminal = episode_over;
    
	return &this_reward_observation;
}

void env_cleanup()
{
	clearRLStruct(&this_observation);
}

const char* env_message(const char* inMessage) {
	if(strcmp(inMessage,"what is your name?")==0)
		return "my name is skeleton_environment!";
    
	return "I don't know how to respond to your message";
}

/*FIX THIS*/
void print_state(){
	int row,col;
    char line[1024];
	sprintf(line, "Agent is at: %d,%d",the_board.agentRow,the_board.agentCol);
    cout << line << endl;
	cout << "Columns:0-10                10-17" << endl;
	cout << "Col    ";
	for(col=0;col<18;col++){
		cout << (col%10) << " ";
	}
    
	for(row=0;row<6;row++){
		cout << endl << "Row: " << row << " ";
		
		for(col=0;col<18;col++){
			if(the_board.agentRow==row && the_board.agentCol==col)
				cout << "A ";
			else{
				if(world_map[row][col]==WORLD_GOAL)
					cout << "G ";
				if(world_map[row][col]==WORLD_MINE)
					cout << "M ";
				if(world_map[row][col]==WORLD_OBSTACLE)
					cout << "* ";
				if(world_map[row][col]==WORLD_FREE)
					cout << "  ";
			}
		}
	}
	cout << endl;
}




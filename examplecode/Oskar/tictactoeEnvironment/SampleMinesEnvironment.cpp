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
	board_state_t is a structure that holds all of the information
	about the world, including the placement of the mines and the position
	of the agent, etc.
*/
typedef struct 
{
  int numRows;		/* Number of rows in the world		*/
  int numCols;		/* Number of columns in the world	*/
  int flatState;
} board_state_t;


// HELPER FUNCTIONS.  PROTOTYPES HERE, CODE AT THE BOTTOM OF FILE */

// Changes the (row,col) position of the agent into a scalar    
int calculate_flat_state(board_state_t aBoard);


// Returns 1 if the current state is terminal (win condition), 0 otherwise 
int check_terminal(int row, int col);


// Returns 1 if the current state is illegal move) 1 if illegal, 0 otherwise
int check_valid(const board_state_t* aBoard,int row, int col);

/*  Calculates the reward in the current state.
	-100 if on a mine
	+10 if at the exit
	-1 otherwise 
*/
double calculate_reward(board_state_t aBoard);

/* Calculates and sets the next state, given an action */
void updatePosition(board_state_t *aBoard, int theAction);


/* Prints out the map to the screen */
void print_state();


/*
	world_map is an array that describes the world.

	To read this: the world is a 6 by 18 grid in any position the number 
	corresponds to one of {START, GOAL, FREE, OBSTACLE, MINE}

	For example in env_init the start position is labelled by a 0
	so we can see the initial start position in this particular map is at
	position [12][1]
	
*/

int world_map[3][3] =
{ 
	{0,0,0},
	{0,0,0},
	{0,0,0}
};


/* GLOBAL VARIABLES FOR RL-GLUE methods (global for convenience) */  
static board_state_t the_board;
static observation_t this_observation;
static reward_observation_terminal_t this_reward_observation;


/* Used if a message is sent to the environment to use fixed start states */
static int fixed_start_state=0;
static int start_row=1;
static int start_col=1;

static string task_spec_string =  
"VERSION RL-Glue-3.0 PROBLEMTYPE episodic \
DISCOUNTFACTOR 1 OBSERVATIONS INTS (0 19682) \
ACTIONS INTS (0 8)  REWARDS (-10.0 -1.0 1.0) \
EXTRA SampleTicTacToeEnvironment(C/C++) by Oskar Lindgren.";

/*****************************

	RL-Glue Methods 
	
*******************************/

const char* env_init(){    

	the_board.numRows = 3;
	the_board.numCols = 3;

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

/* Sets state and returns 1 if valid, 0 if invalid or terminal */
int set_agent_state(int row, int col){
	the_board.agentRow=row;
	the_board.agentCol=col;
    
	
	return check_valid(&the_board,row,col) && !check_terminal(row,col);
}

/* This function sets lastMoveRow and -Col for the AI, the AI dont place illegal moves (check with set_agent_state)*/
void set_random_state(){
	int lastMoveRow=rand()%3;
	int lastMoveCol=rand()%3;

	while(!set_agent_state(lastMoveRow,lastMoveCol)){
		lastMoveRow=rand()%3;
		lastMoveCol=rand()%3;
	}
}
/*
	Standard RL-Glue method. Sets an initial state and returns
	the corresponding observation.
*/
const observation_t *env_start()
{ 
	this_observation.intArray[0]=calculate_flat_state(the_board);
  	return &this_observation;
}


const reward_observation_terminal_t *env_step(const action_t *this_action)
{
	/* Make sure the action is valid (within number of actions ) */
	assert(this_action->numInts==1);
	assert(this_action->intArray[0]>=0);
	assert(this_action->intArray[0]<9);

	updatePosition(&the_board,this_action->intArray[0]);
	this_reward_observation.observation->intArray[0] = calculate_flat_state(the_board);
	this_reward_observation.reward = calculate_reward(the_board);
	this_reward_observation.terminal = check_terminal(the_board.agentRow,the_board.agentCol);

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
 	 * 'set-start-state X Y'
	 * Action: Set flag to do fixed starting states (row=X, col=Y)
	 */
	if(inMessage.substr(0,15) == "set-start-state"){
	 	{
      string suffix = inMessage.substr(16);
      int row, col; 
      stringstream sstr; 
      sstr << suffix; 
      sstr >> row; 
      sstr >> col; 
		
			start_row = row; 
			start_col = col; 

      fixed_start_state=1;

      return "Message understood.  Using fixed start state.";
   	}
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

int calculate_flat_state(board_state_t aBoard){
	int state = 0;
    int posCounter = 1;
    for (int x = 0;x<numCols;x++){
        for (int y = 0; y<numRows;y++){
            state += posCounter*aBoard[x][y];
            posCounter*=3;
        }
    }
    
    return state;
}

int check_illegal(int row, int col){
	if (world_map[row][col] != 0){
		return 1;
	}
	return 0;
}

int check_win(const board_state_t* aBoard, int row, int col){
	int winwin = 0;
    int win=1;
	for (int i = 0; i<3 ; i++){
        if(world_map[row][i] != world_map[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1};
    
    win=1;
    for (int i = 0; i<3 ; i++){
        if(world_map[i][col] != world_map[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1};

    win=1;
    for (int i = 0; i<3 ; i++){
        if(world_map[i][i] != world_map[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1};
    
    win=1;
    for (int i = 0; i<3 ; i++){
        if(world_map[i][2-i] != world_map[row][col]){
            win = 0;
        }
    }
    if(win==1){winwin=1};
    
	return winwin;
}

double calculate_reward(board_state_t aWorld){
	if(world_map[aWorld.agentRow][aWorld.agentCol] == WORLD_GOAL){
		return 10.0;
	}

	if(world_map[aWorld.agentRow][aWorld.agentCol] == WORLD_MINE){
		return -100.0;
	}
	
	return -1.0;
}

void updatePosition(board_state_t *aBoard, int theAction){
	// When the move would result in hitting an obstacles,
  // the agent simply doesn't move 
	int moveRow;
	int moveCol;
    
    moveRow = theAction/3;
    moveCol = theAction%3;

	//Check if new position is out of bounds or inside an obstacle 
	if(check_valid(aBoard,newRow,newCol)){
   		aWorld->agentRow = newRow;
   		aWorld->agentCol = newCol;
	}
}

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


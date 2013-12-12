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
 Tic tac toe, agent vs an ai that plays random legal moves. If the player
 plays an illegal (occupied) move, the agent gets a penalty of -10 and is
 allowed to continue. If the ai wins, the agent gets a penalty of -1, if
 the agent wins, it gets a reward of 1. If someone wins, the game(episode)
 is over.
 */

// HELPER FUNCTIONS.  PROTOTYPES HERE, CODE AT THE BOTTOM OF FILE */

int is_win(int row, int col);

int is_illegal_move(int a);

int ai_random_move_is_win();

int flatten_state();

int is_legal_moves_left();

void print_state();

void print_cause(int cause, int row, int col);

static observation_t this_observation;
static reward_observation_terminal_t this_reward_observation;

int current_state=0;
int print_after_each_reward = 0;
int multiplier[7] = {1,3, 9, 27, 81, 244, 729};
const int nRows = 6;
const int nCols = 7;
int current_board[nRows][nCols] =
{
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0}
};

/* Used if a message is sent to the environment to use fixed start states */

static string task_spec_string =  
"VERSION RL-Glue-3.0 PROBLEMTYPE episodic \
DISCOUNTFACTOR 1 OBSERVATIONS INTS (0 2186) \
ACTIONS INTS (0 6)  REWARDS (0. 1.) \
EXTRA tictactoe_environment(C/C++) by Oskar Lindgren, Oscar Carlsson, John Karlsson";

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
	int a=rand()%nCols;
    int row = 0;

	while(is_illegal_move(a)){
		a=rand()%nCols;
	}
    // TODO: Row always set? 
    for (int i = nRows-1; i >= 0; i--) {
       if (current_board[i][a] == 0 ) {
           row = i;
           current_board[i][a] = 2;
           break;
       }
    }
    if(is_win(row,a)){
        return 1;
    }
    return 0;
}

int flatten_state(){
    current_state = 0;
    for(int i = 0; i<nCols;i++){
        for(int j = 0; j<nRows;j++){
            if (current_board[j][i] != 0){
                current_state+=multiplier[i]*current_board[j][i];
                break;
            }
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
    
    for (int i = 0; i<nRows; i++) {
        for (int j=0; j<nCols; j++) {
            current_board[i][j] = 0;
        }
    }
    if(rand()%1){
        ai_random_move_is_win();
        current_state=flatten_state();
    }else{
        current_state=0;
    }

	this_observation.intArray[0]=current_state;
  	return &this_observation;
}

int is_legal_moves_left(){
    for(int i= 0; i<nCols; i++){
        if (current_board[0][i] == 0){
            return 1;
        }
    }
    return 0;
}

const reward_observation_terminal_t *env_step(const action_t *this_action)
{
	/* Make sure the action is valid */
	assert(this_action->numInts==1);
	assert(this_action->intArray[0]>=0);
	assert(this_action->intArray[0]<nCols);
    
    int row;
    int a = (this_action->intArray[0]);
    int col = a;

    int episode_over = 0;
    double the_reward=0;
    int eventCode = 0;
   

    if(is_illegal_move(a)){
		episode_over=0;
        eventCode = 1;
		the_reward=0.;
        row = -1;
    }else{
        // Make action 
        for (int i = nRows-1; i >= 0; i--) {
           if (current_board[i][a] == 0 ) {
               row = i;
               col = a;
               current_board[i][a] = 1;
               break;
           }
        }
        
        if(is_win(row,col) || !is_legal_moves_left()){
            episode_over=1;
            eventCode = 2;//agent won by 3 in row or by filling board
            the_reward=0;
            if (is_win(row,col))
                the_reward = 1;
        }else{
            if(ai_random_move_is_win()){
                current_state=0;
                episode_over=1;
                eventCode = 3;//ai won by 3 in row
                the_reward=0;
            }
        }
    }
    if(eventCode>0 && print_after_each_reward){
        print_cause(eventCode, row, col);
    }
  
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
        return "Message understood.  Start state is an empty board though.";
    }
	
    if(inMessage == "print-after-each-reward") {
        print_after_each_reward = 1;
        return "Message understood.  Printing after reward.";
    }
    
    if(inMessage == "dont-print-after-each-reward") {
        print_after_each_reward = 0;
        return "Message understood.  No more printing after reward.";
    }
	/*	Message Description
 	 * 'print-state'
	 * Action: Print the map and the current agent location
	 */
	if(inMessage == "print-state"){
		print_state();
		return "Message understood.  Printed the state.";
  }

	return "TicTacToe (C++) does not respond to that message.";
}


/*****************************

	Helper Methods 
	
*******************************/

int is_win(int row, int col){
    int player = current_board[row][col];

    // Horizontal
    int inarow = 0;
    for (int i = 0; i < nCols; i++) {
       if(current_board[row][i] == player) {
        inarow++;
        if (inarow == 4){
            return 1;
        }
       }else{
        inarow = 0;
       }
    }
    
    // Vertical
    inarow = 0;
    for (int i = 0; i < nRows; i++) {
       if(current_board[i][col] == player) {
        inarow++;
        if (inarow == 4){
            return 1;
        }
       }else{
        inarow = 0;
       }
    }

    // Diag1
    inarow = 0;
    int c_row = row;
    int c_col = col;
    while(++c_row < nRows && ++c_col< nCols){
       if (current_board[c_row][c_col] == player){
        inarow++; 
       }else{
        break;
       }
    }
    c_row = row;
    c_col = col;
    while(--c_row >= 0 && --c_col >= 0){
       if (current_board[c_row][c_col] == player){
        inarow++; 
       }else{
        break;
       }
    }

    if (inarow >= 4){
        return 1; // winning diagonal
    }

    // Diag1
    inarow = 0;
    c_row = row;
    c_col = col;
    while(++c_row < nRows && --c_col >= 0){
       if (current_board[c_row][c_col] == player){
        inarow++; 
       }else{
        break;
       }
    }
    c_row = row;
    c_col = col;
    while(--c_row >= 0 && ++c_col < nCols){
       if (current_board[c_row][c_col] == player){
        inarow++; 
       }else{
        break;
       }
    }

    if (inarow >= 4){
        return 1; // winning diagonal
    }
    return 0;
}

int is_illegal_move(int a){
	if (current_board[0][a] != 0){
        return 1;
	}
	return 0;
}
void print_cause(int cause, int row, int col){
    if(cause == 1){
        print_state();
        cout<<"Agent attempted illegal move: row:"<<row+1<<", col:"<<col+1<<"\n";
    }
    if(cause == 2){
        if(is_legal_moves_left()){
            print_state();
            cout<<"Agent won by 3 in a row, last move: row:"<<row+1<<", col:"<<col+1<<"\n";
        }else{
            print_state();
            cout<<"Agent won by filling board, no legal moves left,\n last move: row:"<<row+1<<", col:"<<col+1<<"\n";
        }
    }
    if(cause == 3){
        print_state();
        cout<<"Agent lost by 3 in a row by ai\n";
    }
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

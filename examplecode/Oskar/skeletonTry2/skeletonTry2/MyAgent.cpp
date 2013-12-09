#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <rlglue/utils/C/RLStruct_util.h>
#include <rlglue/Agent_common.h>
#include <rlglue/utils/C/TaskSpec_Parser.h>
#include <vector>
#include <time.h>
#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;

/*
 * CODE       : an RL-Agent for any environment ;)
 * KEYWORDS   : Sarsa-learning, Eligibility traces, Sparse distributed memory, 
 *              Fuzzy membership, Adaptative Prototypes, Sampling
 * DESIGN     : Learning : Online Temporal difference Sarsa & Eligibility traces
 *              Exploration-Exploitation : Sampling & Epsilon-greedy
 *              Space representation : SDM, fuzzy membership & sampling for discrete and continuous space
 *              Tuning : Prototype adaptation           
 * COMPLEXITY : O(N*(D+M)), where N = # prototypes , M = # action samples , D = # dimensions
 *              Choose N = C/D to get O(N*M) complexity independent of the environment, C being a constant
 *              An upper bound on the complexity is then O(C*M), both parameters being har-coded, hence the
 *              Agent's complexity seen from the environment is constant O(1)!
 * AUTHORS    : Christian Kauth (christian.kauth@epfl.ch) & Mohammadjavad Faraji (mohammadjavad.faraji@epfl.ch)
 */

typedef vector<double> TState;		// a state is described by 1 double per dimension of state-space
typedef vector<double> TAction;		// an action is described by 1 double per dimension of action-space
typedef vector<double> TStact;		// a state-action pair is described 1 double per dimension of state-action-space

// Decodes the observation (received from the environment)
TState decode_observation(const observation_t *this_observation);
// Encodes the action (to be sent to the environment)
action_t *encode_action(TAction act);
// Generates a valid random action
TAction rand_action();
// Decides on the action to take
TAction decide_action(TState state);
// Evaluates a state-action pair of the space via fuzzy feature coding
double state_action_value(TState state, TAction act);
// Computes the membership grades of a state-action pair to prototypes
void state_action_projection(TState state, TAction act);
// Generates the initial, regularly distributed observation-action feature set for SDM coding
void prototypes_init(double p, int d);
// Places the prototypes regularly over the state-action space
void prototypes_regular(TStact &pos, double p, int d);
// Computes the normalized distance between two prototypes
double prototypes_dist(int a, int b);
// Enhances the prototype set
void prototypes_shuffle();
// print the prototypes
void prototypes_print();

#define NM_PRODUCT 10000 	// maximum (#prototypes describing space value)*(dimensionality of space)
#define N_ACTIONS 10		// number of actions considered by policy
#define EPSILON_ZERO 0.1	// greedy action part initial (exploitation-exploration trade-off)
#define EPSILON_DECAY 1		// greedy action part decay (exploitation-exploration trade-off)
#define ALPHA 0.2			// gradient descent step for function approximation
#define LAMBDA 0.9			// for Sarsa(Lambda) eligibility traces
#define DEBUG_MODE 0		// 0 for normal run, 1 for debugging
#define REPORT_MODE 1		// 0 for normal mode, 1 for reporting prototype placement

/* dimensions span spaces */
struct TDim{
	double min, max;
	bool isDiscrete;
	TDim () {};
	TDim (double mi, double ma, bool di) : min(mi), max(ma), isDiscrete(di) {};
};

/* fuzzy prototypes encode the observation-action value of the space */
struct TPrototype{
	vector<double> pos;
	double sigma;
	double value;
	double popularity;
	double eligibility;
	double weight;
	double sDist1;
	double sDist2;
	TPrototype(vector<double> p) : pos(p) , value(0), popularity(0), eligibility(0), sDist1(0), sDist2(0) {};
};

action_t *this_action=0;				// action to be sent to the environment
observation_t *last_observation=0;		// observation received from the environment
int N_PROTOTYPES;						// number of prototypes describing space value
double df;								// discount factor
vector<TDim> oSpace;					// observation space
vector<TDim> aSpace;					// action space
vector<TDim> oaSpace;					// observation-action space
TDim rSpace;							// reward space
vector<TPrototype> prototypes;			// prototypes spanning the state-action space
TState sOld, sNew;						// old and new states
TAction aOld, aNew;						// old and new actions
double EPSILON;							// greedy action part (exploitation-exploration trade-off)

/*
 * Initializes the agent by parsing the task specifications
 */
void agent_init(const char* task_spec)
{
	// spin the wheel of fortune
	srand(time(NULL));
	// Parse task specifications
	taskspec_t *ts=(taskspec_t*)malloc(sizeof(taskspec_t));
	int decode_result = decode_taskspec( ts, task_spec );
	if(decode_result!=0){
		printf("Could not decode task spec, code: %d for task spec: %s\n",decode_result,task_spec);
		exit(1);
	}
	df = ts->discount_factor;
	oSpace.clear();
	for (int i=0; i<ts->num_int_observations; i++)
		for (int j=0; j<(ts->int_observations+i)->repeat_count; j++)
			oSpace.push_back(TDim((ts->int_observations+i)->min,(ts->int_observations+i)->max,true));
	for (int i=0; i<ts->num_double_observations; i++)
		for (int j=0; j<(ts->double_observations+i)->repeat_count; j++)
			oSpace.push_back(TDim((ts->double_observations+i)->min,(ts->double_observations+i)->max,false));
	aSpace.clear();
	for (int i=0; i<ts->num_int_actions; i++)
		for (int j=0; j<(ts->int_actions+i)->repeat_count; j++)
			aSpace.push_back(TDim((ts->int_actions+i)->min,(ts->int_actions+i)->max,true));
	for (int i=0; i<ts->num_double_actions; i++)
		for (int j=0; j<(ts->double_actions+i)->repeat_count; j++)
			aSpace.push_back(TDim((ts->double_actions+i)->min,(ts->double_actions+i)->max,false));
	oaSpace = oSpace;
	oaSpace.insert(oaSpace.end(),aSpace.begin(),aSpace.end());
	rSpace = TDim(ts->reward.min,ts->reward.max,false);
		
	// some debugging		
	if (DEBUG_MODE){
		printf("Discount factor : %lf\n",df);
		printf("Observation space : %d\n",oSpace.size());
		for (int i=0; i<oSpace.size(); i++)
			printf("[ %lf ; %lf ] - %s\n", oSpace[i].min, oSpace[i].max, oSpace[i].isDiscrete ? "discrete" : "continuous" );
		printf("Action space : %d\n",aSpace.size());
		for (int i=0; i<aSpace.size(); i++)
			printf("[ %lf ; %lf ] - %s\n", aSpace[i].min, aSpace[i].max, aSpace[i].isDiscrete ? "discrete" : "continuous" );
		printf("Reward space : %d\n",1);
			printf("[ %lf ; %lf ] - %s\n", rSpace.min, rSpace.max, rSpace.isDiscrete ? "discrete" : "continuous" );
		printf("%s\n",task_spec);
	}
	
	// Initialize prototype space
	N_PROTOTYPES = 0;
	prototypes_init((NM_PRODUCT*1.0)/oaSpace.size(),oaSpace.size());
	
	// Allocate memory for actions
	this_action = allocateRLStructPointer(getNumIntAct(ts),getNumDoubleAct(ts),0);
}

/*
 * Binary-searches for maximum number of prototypes per unconstrained dimension
 */
 int prototypes_per_dimension(int maxP){
 	int l(1), r(NM_PRODUCT), x;
 	double cnt;
 	while (l<r){
 		x = (l+r)/2;
 		cnt = 1;
 		for (int i=0; i<oaSpace.size(); i++)
 			if (oaSpace[i].isDiscrete)
 				cnt *= min( x*1.0 , oaSpace[i].max-oaSpace[i].min+1 );
 			else
 				cnt *= x;
 		(floor(cnt) < maxP) ? l=x+1 : r=x;
 	}
 	return x;
 }

/*
 * Places the prototypes regularly over the state-action space
 */
void prototypes_regular(vector<double> &pos, double p, int d){
	int n = p;
	int j = pos.size()-d;
	if (oaSpace[j].isDiscrete)
		n = min( n*1.0, oaSpace[j].max-oaSpace[j].min+1 );
	if (d==0){
		prototypes.push_back(TPrototype(pos));
		return;
	}
	for (int i=0; i<n; i++){	
		pos[j] = oaSpace[j].min + (oaSpace[j].max-oaSpace[j].min)*(i+0.5)/n;
		prototypes_regular(pos,p,d-1);
	}
}

/*
 * Computes the normalized distance between two prototypes
 */
double prototypes_dist(int a, int b){
	double dist(0);
	for (int i=0; i<oaSpace.size(); i++)
		dist += pow( (prototypes[a].pos[i]-prototypes[b].pos[i]) / (oaSpace[i].max-oaSpace[i].min), 2.0 );
	return sqrt(dist);
}

/*
 * Generates the initial, observation-action prototype set for SDM coding
 */
void prototypes_init(double p, int d){
	vector<double> pos(d,0);
	double sD1(0), sD2(0), dist, sigma(0);
	
	// create prototypes
	prototypes.clear();
	int ppd = prototypes_per_dimension(p);
	prototypes_regular(pos,ppd,d);
	N_PROTOTYPES = prototypes.size();
	
	// some reporting
	if (REPORT_MODE){
		ofstream report("rep/prototypes.txt");
		for (int i=0; i<N_PROTOTYPES; i++){
			for (int j=0; j<oaSpace.size(); j++)
				report<<prototypes[i].pos[j]<<" ";
			report<<endl;
		}
		report.close();
	}
	
	// compute prototype action range
	for (int i=1; i<N_PROTOTYPES; i++){
		dist = prototypes_dist(0,i);
		sD1 += dist;
		sD2 += dist*dist;
	}
	for (int i=0; i<N_PROTOTYPES; i++){
		prototypes[i].sDist1 = sD1;
		prototypes[i].sDist2 = sD2;
		prototypes[i].sigma =  (sD2 - 2*sD1*sD1/N_PROTOTYPES + sD1) / N_PROTOTYPES;
	}
		
	// reset EPSILON for greedy choice
	EPSILON = EPSILON_ZERO;
	
	// some debugging	
	if (DEBUG_MODE){
		for (int i=0; i<N_PROTOTYPES; i++){
			printf("pos : ");
			for (int j=0; j<prototypes[i].pos.size(); j++)
				printf("%.3lf ",prototypes[i].pos[j]);
			printf(" - sig : %.3lf\n",prototypes[i].sigma);
		}
	}
}

/*
 * Enhances the prototype set by replacing the least popular prototype
 */
void prototypes_shuffle(){
	// find least polular prototype
	int unPop(0);
	for (int i=1; i<N_PROTOTYPES; i++)
		if (prototypes[i].popularity < prototypes[unPop].popularity)
			unPop = i;
	
	// de-embed the unpopular prototype	
	for (int i=0; i<N_PROTOTYPES; i++){
		double dist = prototypes_dist(unPop,i);
		prototypes[i].sDist1 -= dist;
		prototypes[i].sDist2 -= dist*dist;
	}	
			
	// create any random prototype (with unchanged popularity)
	vector<double> sta, act;
	for (int i=0; i<oSpace.size(); i++){
		double m(0);
		for (int j=0; j<N_PROTOTYPES; j++)
			m += prototypes[j].pos[i];
		prototypes[unPop].pos[i] = m/N_PROTOTYPES;
		//prototypes[unPop].pos[i] = oSpace[i].min + (oSpace[i].max-oSpace[i].min)*rand()/RAND_MAX;
		sta.push_back( prototypes[unPop].pos[i] );
	}
	for (int i=0; i<aSpace.size(); i++){
		prototypes[unPop].pos[i+oSpace.size()] = aSpace[i].min + (aSpace[i].max-aSpace[i].min)*rand()/RAND_MAX;
		act.push_back( prototypes[unPop].pos[i+oSpace.size()] );
	}
	prototypes[unPop].value = state_action_value(sta,act);
	prototypes[unPop].eligibility = 0;
	prototypes[unPop].sDist1 = 0;
	prototypes[unPop].sDist2 = 0;
	for (int i=0; i<N_PROTOTYPES; i++){
		double dist = prototypes_dist(unPop,i);
		prototypes[unPop].sDist1 += dist;
		prototypes[unPop].sDist2 += dist*dist;
	}
	prototypes[unPop].sigma = (prototypes[unPop].sDist2 - 2*prototypes[unPop].sDist1*prototypes[unPop].sDist1/N_PROTOTYPES + prototypes[unPop].sDist1) / N_PROTOTYPES;
	
	// embed the new prototype
	for (int i=0; i<N_PROTOTYPES; i++){
		double dist = prototypes_dist(unPop,i);
		prototypes[i].sDist1 += dist;
		prototypes[i].sDist2 += dist*dist;
		prototypes[i].sigma =  (prototypes[i].sDist2 - 2*prototypes[i].sDist1*prototypes[i].sDist1/N_PROTOTYPES + prototypes[i].sDist1) / N_PROTOTYPES;
		prototypes[i].popularity = 0;
	}
}

/*
 * Prints the prototypes
 */
void prototypes_print(){
	if (DEBUG_MODE)
		for (int i=0; i<N_PROTOTYPES; i++){
			printf("pos : ");
			for (int j=0; j<prototypes[i].pos.size(); j++)
				printf("%.3lf ",prototypes[i].pos[j]);
			printf(" - sig : %.3lf - val : %.3lf - eli : %.3lf - wei : %.3lf - pop : %.3lf\n", prototypes[i].sigma, prototypes[i].value, prototypes[i].eligibility, prototypes[i].weight, prototypes[i].popularity);
		}		
}

/*
 * Decodes the observation
 */
TState decode_observation(const observation_t *this_observation){
	TState state;
	for (int i=0; i<this_observation->numInts; i++) state.push_back(this_observation->intArray[i]);
	for (int i=0; i<this_observation->numDoubles; i++) state.push_back(this_observation->doubleArray[i]);
	return state;
}

/*
 * Encodes the action
 */
action_t *encode_action(TAction act){
	int ia(0), da(0);
	for (int i=0; i<act.size(); i++)
		if (aSpace[i].isDiscrete)
			this_action->intArray[ia++] = act[i];
		else
			this_action->doubleArray[da++] = act[i];
	return this_action;
}

/*
 * Generates a valid random action
 */
TAction rand_action(){
	TAction act;
	for (int i=0; i<aSpace.size(); i++)
		if (aSpace[i].isDiscrete)
			act.push_back( floor( aSpace[i].min + (aSpace[i].max-aSpace[i].min)*rand()/RAND_MAX + 0.5 ) );
		else
			act.push_back( aSpace[i].min + (aSpace[i].max-aSpace[i].min)*rand()/RAND_MAX );
	return act;
}

/*
 * Decides on the action to take in a sampled epsilon-greedy way
 */
TAction decide_action(TState state){
	TAction bestAct( rand_action() );
	double bestVal ( state_action_value( state, bestAct ) );
	if (rand()/(1.0*RAND_MAX) > EPSILON)
		for (int i=0; i<N_ACTIONS; i++){
			TAction thisAct = rand_action();
			double thisVal = state_action_value( state, thisAct );
			if ( thisVal > bestVal ){
				bestAct = thisAct;
				bestVal = thisVal;
			}
		}
	return bestAct;
}

/*
 * Evaluates a state-action pair of the space via fuzzy prototype coding
 */
double state_action_value(TState state, TAction act){
	state_action_projection( state, act );
	double Q(0), M(0), P(0);
	for (int i=0; i<N_PROTOTYPES; i++){
		Q += prototypes[i].weight * prototypes[i].value;
		P += prototypes[i].popularity * prototypes[i].weight;
		M += prototypes[i].weight;
	}
	return Q/M;
}

/*
 * Computes the membership grades of a state-action pair to prototypes
 */
void state_action_projection(TState sta, TAction act){
	for (int i=0; i<N_PROTOTYPES; i++){
		prototypes[i].weight = 0;
		for (int j=0; j<sta.size(); j++)
			prototypes[i].weight += pow( (prototypes[i].pos[j]-sta[j]) / (oSpace[j].max-oSpace[j].min), 2.0 );
		for (int j=0; j<act.size(); j++)
			prototypes[i].weight += pow( (prototypes[i].pos[j+sta.size()]-act[j]) / (aSpace[j].max-aSpace[j].min), 2.0 );
		prototypes[i].weight = exp( -prototypes[i].weight/(2*prototypes[i].sigma) );
	}
}

/*
 * Chooses agent's first action
 */
const action_t *agent_start(const observation_t *this_observation) {
	sNew = decode_observation(this_observation);
	aNew = decide_action(sNew);
	
	// some debugging
	if (DEBUG_MODE){
		printf("A: - @ observation ");
		for (int i=0; i<sNew.size(); i++)
			printf("%.3lf ",sNew[i]);
		printf("\n");
	}
	
	// decay EPSILON for greedy choice
	EPSILON *= EPSILON_DECAY;
	
	return encode_action( aNew );
}

/*
 * Decides on next action to take given a reward and observation
 * - Sampled Epsilon-greedy decision policy
 * - Sarsa-Lambda reinforcement learning
 * - fuzzy and adaptative SDM coding
 */
const action_t *agent_step(double reward, const observation_t *this_observation) {
	sOld = sNew;
	aOld = aNew;
	sNew = decode_observation(this_observation);
	aNew = decide_action(sNew);
	
	// some debugging
	if (DEBUG_MODE){
		printf("A: - reward %.3lf @ observation ",reward);
		for (int i=0; i<sNew.size(); i++)
			printf("%.3lf ",sNew[i]);
		printf("\n");
	}
	
	// Sarsa learning
	state_action_projection( sOld, aOld );
	double delta = reward + df*state_action_value(sNew,aNew) - state_action_value(sOld,aOld);
	for (int i=0; i<N_PROTOTYPES; i++){
		prototypes[i].eligibility = min( df*LAMBDA*prototypes[i].eligibility + prototypes[i].weight , 1.0 );
		prototypes[i].value += ALPHA * delta * prototypes[i].eligibility;
		prototypes[i].popularity += prototypes[i].weight;
	}

	return encode_action( aNew );
}

/*
 * Evaluates agent's final reward
 */
void agent_end(double reward) {
	sOld = sNew;
	aOld = aNew;
	state_action_projection( sOld, aOld );
	double delta = reward + df*0 - state_action_value(sOld,aOld);
	for (int i=0; i<N_PROTOTYPES; i++){
		prototypes[i].eligibility = min( df*LAMBDA*prototypes[i].eligibility + prototypes[i].weight , 1.0 );
		prototypes[i].value += ALPHA * delta * prototypes[i].eligibility;
		prototypes[i].popularity += prototypes[i].weight;
	}
	
	// adapts the prototypes
	//prototypes_shuffle();
	prototypes_print();
	
	// some reporting
	if (REPORT_MODE){
		ofstream report("rep/prototypes_after.txt");
		for (int i=0; i<N_PROTOTYPES; i++){
			for (int j=0; j<oaSpace.size(); j++)
				report<<prototypes[i].pos[j]<<" ";
			report<<endl;
		}
		report.close();
	}
}

void agent_cleanup() {
	clearRLStruct(this_action);
	freeRLStructPointer(last_observation);
}

void agent_freeze() {
}

const char* agent_message(const char* inMessage) {
	if(strcmp(inMessage,"what is your name?")==0)
		return "my name is myAgent!";
	return "I don't know how to respond to your message";
}

/** 
	Copyright (C) 2008, Brian Tanner
	          (C) 2013, Christos Dimitrakakis.			  

			  Distributed under the GNU Public License 3.0.
			  (originally Apache License v2.0).
 */

#include <cstdio>    // for sprintf
#include <cmath>     // for sqrt

#include <iostream>   // for cout
#include <fstream>   
#include <string>

#include <vector>

#include <rlglue/RL_glue.h> /* RL_ function prototypes and RL-Glue types */

using namespace std; 

/*
* Experiment program that does some of the things that might be important when
* running an experiment.  It runs an agent on the environment and periodically
* asks the agent to "freeze learning": to stop updating its policy for a number
* of episodes in order to get an estimate of the quality of what has been learned
* so far.
*
* The experiment estimates statistics such as the mean and standard deviation of
* the return gathered by the policy and writes those to a comma-separated value file
* called results.csv.
*
* This experiment also shows off some other features that can be achieved easily
* through the RL-Glue env/agent messaging system by freezing learning (described
* above), having the environment start in specific starting states, and saving
* and loading the agent's value function to/from a binary data file.
*/

class EpisodeStatistics
{
public:
	std::vector<double> episode_reward;
	std::vector<int> episode_length;
	double total_reward;
	double total_steps;
	EpisodeStatistics() : total_reward(0.0)
	{
	}
	void AddEpisode(double reward, int length)
	{
		total_reward += reward;
		total_steps += length;
		episode_reward.push_back(reward);
		episode_length.push_back(length);
	}
};

class RunStatistics
{
public:
	std::vector<EpisodeStatistics> runs;
	int n_runs;
	double total_reward;
	RunStatistics() : n_runs(0), total_reward(0.0)
	{
	}
	void AddRun(EpisodeStatistics& statistics)
	{
		runs.push_back(statistics);
		total_reward += statistics.total_reward;
		n_runs++;
	}
	void PrintStatistics()
	{
		int max_episodes = 0;
		for (int i=0; i<n_runs; ++i) {
			if (max_episodes < (int) runs[i].episode_reward.size()) {
				max_episodes = runs[i].episode_reward.size();
			}
		}
		std::vector<double> episode_reward(max_episodes);
		for (int i=0; i<max_episodes; ++i) {
			int active_runs = 0;
			episode_reward[i] = 0;
			for (int j=0; j<n_runs; ++j) {
				if (j < ((int) runs[j].episode_reward.size()))
					active_runs++;
				episode_reward[i] += runs[j].episode_reward[i];
			}
			std::cout << episode_reward[i] / (double) active_runs
					  << " # episode reward"
					  << std::endl;
		}
		std::cout << total_reward / (double) n_runs
				  << " # average total reward over runs"
				  << std::endl;
	}

    void save_result_csv(const string & fileName) {

        ofstream output(fileName.c_str(), ios::app); 
        if (!output.is_open())
            cerr << "Error opening file for reading: " << fileName << endl; 

		int max_episodes = 0;
		for (int i=0; i<n_runs; ++i) {
			if (max_episodes < (int) runs[i].episode_reward.size()) {
				max_episodes = runs[i].episode_reward.size();
			}
		}
        output << "mean,";
		std::vector<double> episode_reward(max_episodes);
		for (int i=0; i<max_episodes; ++i) {
			int active_runs = 0;
			episode_reward[i] = 0;
			for (int j=0; j<n_runs; ++j) {
				if (j < ((int) runs[j].episode_reward.size()))
					active_runs++;
				episode_reward[i] += runs[j].episode_reward[i];
			}
            output << episode_reward[i] / (double) active_runs;
            if(i != max_episodes-1){
                output << ",";
            }
        }
        output << endl;

        output.close(); 
    }
};


EpisodeStatistics online_evaluation(int n_episodes);


string file_name;
int main(int argc, char *argv[]) {
    if (argc > 1){
        std::cout << argv[1] << std::endl;
        file_name = argv[1];
    }else{
        std::cout << "result" << std::endl;    
        file_name = "result";
    }

	std::cout << "Starting online demo" << std::endl
			  << "----------------------------" << std::endl
			  << std::endl << std::endl; 

	int n_runs = 1;
	int n_episodes = 1000;
	RunStatistics run_statistics;
	for (int i=0; i<n_runs; ++i) {
		std::cout << "Run " << i << std::endl;
		RL_init();
		EpisodeStatistics episode_statistics = online_evaluation(n_episodes);
		run_statistics.AddRun(episode_statistics);
		RL_cleanup();
	}
	run_statistics.PrintStatistics();
    run_statistics.save_result_csv(file_name);
	std::cout << "\nProgram Complete.\n";

	return 0;
}


/** Online evaluation

*/
EpisodeStatistics  online_evaluation(int n_episodes)
{
	EpisodeStatistics statistics;
	for(int i=0; i<n_episodes; i++){
		RL_episode(1000);
		double reward = RL_return();
		int steps = RL_num_steps();
		statistics.AddEpisode(reward, steps);
	}
	return statistics;
}

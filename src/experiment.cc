#include <iostream>
#include <functional>
#include <getopt.h>
#include "coco.h"
#include "differentialevolution.h"
#include "params.h"

static coco_problem_t *PROBLEM;
static int BUDGET_MULTIPLIER = 1e5;
static int INDEPENDENT_RUNS = 20;
static std::vector<std::string> INSTANCES = {"1","2","3","4","5"};

#include "default_params.h"

std::string gen_instances(){
	std::string instances = "";
	for (std::string i : INSTANCES)
		for (int j = 0; j < INDEPENDENT_RUNS; j++)
			instances += i + ",";
	instances.pop_back();
	return instances;
}

void experiment(DifferentialEvolution& de,
				char const *const suite_name,
				char const *const suite_options,
				char const *const observer_name,
				char const *const observer_options) {

	coco_suite_t *suite;
	coco_observer_t *observer;
	suite = coco_suite(suite_name, ("instances: " + gen_instances()).c_str(), suite_options);
	observer = coco_observer(observer_name, observer_options);

	while ((PROBLEM = coco_suite_get_next_problem(suite, observer))) {
		int const dimension = coco_problem_get_dimension(PROBLEM);
		int const popSize = dimension * params::popsize_multiplier;
		size_t const budget = dimension * BUDGET_MULTIPLIER;

		do {
			de.run(PROBLEM, budget, popSize);
		} while (!coco_problem_final_target_hit(PROBLEM) && coco_problem_get_evaluations(PROBLEM) < budget);
	}

	coco_observer_free(observer);
	coco_suite_free(suite);
}

std::vector<std::string> splitString(std::string str){
	std::vector<std::string> v; 
	size_t pos = 0;
	while ((pos = str.find(',')) != std::string::npos) {
		std::string const token = str.substr(0, pos);
		v.push_back(token);
		str.erase(0, pos + 1);
	}
	v.push_back(str);
	return v;
}

int main(int argc, char** argv){
	std::string 
		strategy    = "C",
		param		= "S",		
		credit		= "CO",
		reward 		= "EA",
		quality 	= "WS",
		probability	= "AP",
		constraint 	= "RS",
	  	dimensions 	= "20",
		functions 	= "1-24",
		id = "DE",
		coco_log_level = "info";

	std::vector<std::string> 
		mutation = {"RA1"}, 
		crossover = {"B"};

	int c;

#define STRATEGY_FLAG 1000
#define PARAMETER_FLAG 1001
#define CREDIT_FLAG 1002
#define REWARD_FLAG 1003
#define QUALITY_FLAG 1004
#define PROBABILITY_FLAG 1005
#define CONSTRAINT_FLAG 1006
#define ID_FLAG 1007
#define ALPHA_FLAG 1008
#define BETA_FLAG 1009
#define GAMMA_FLAG 1010
#define POPSIZE_MULTIPLIER_FLAG 1011
#define LOG_ACTIVATIONS_FLAG 1012
#define LOG_PARAMETERS_FLAG 1013
#define LOG_POSITIONS_FLAG 1014
#define LOG_DIVERSITY_FLAG 1015
#define LOG_REPAIRS_FLAG 1016
#define BUDGET_MULTIPLIER_FLAG 1017
#define INDEPENDENT_RUNS_FLAG 1018
#define COCO_LOG_LEVEL_FLAG 1019

	while(true){	
		static struct option long_options[] =
		{
			{"dimensions", required_argument, 0, 'd'},
			{"functions", required_argument, 0, 'f'},
			{"instances", required_argument, 0, 'i'},
			{"mutation", required_argument, 0, 'm'},
			{"crossover", required_argument, 0, 'c'},
			{"Cr", required_argument, 0, 'C'},

			{"strategy", required_argument, 0, STRATEGY_FLAG},
			{"parameter", required_argument, 0, PARAMETER_FLAG},
			{"credit", required_argument, 0, CREDIT_FLAG},
			{"reward", required_argument, 0, REWARD_FLAG},
			{"quality", required_argument, 0, QUALITY_FLAG},
			{"probability", required_argument, 0, PROBABILITY_FLAG},
			{"constraint", required_argument, 0, CONSTRAINT_FLAG},
			{"id", required_argument, 0, ID_FLAG},
			{"alpha", required_argument, 0, ALPHA_FLAG},
			{"beta", required_argument, 0, BETA_FLAG},
			{"gamma", required_argument, 0, GAMMA_FLAG},
			{"popsize-multiplier", required_argument, 0, POPSIZE_MULTIPLIER_FLAG},
			{"budget-multiplier", required_argument, 0, BUDGET_MULTIPLIER_FLAG},
			{"independent-runs", required_argument, 0, INDEPENDENT_RUNS_FLAG},
			{"coco-log-level", required_argument, 0, COCO_LOG_LEVEL_FLAG},

			{"log-activations", optional_argument, 0, LOG_ACTIVATIONS_FLAG},
			{"log-parameters", optional_argument, 0, LOG_PARAMETERS_FLAG},
			{"log-positions", optional_argument, 0, LOG_POSITIONS_FLAG},
			{"log-diversity", optional_argument, 0, LOG_DIVERSITY_FLAG},
			{"log-repairs", optional_argument, 0, LOG_REPAIRS_FLAG},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		c = getopt_long (argc, argv, "d:f:i:m:c:F:C:", long_options, &option_index);

		if (c == -1)
			break;

#define OPT(X,Y) case X: Y = optarg; break;
		switch (c){
			case 'd': dimensions = optarg; break;
			case 'f': functions = optarg; break;
			case 'i': INSTANCES = splitString(optarg); break;
			case 'm': mutation = splitString(optarg); break;
			case 'c': crossover = splitString(optarg); break;
			case 'F': params::F = std::stod(optarg); break;
			case 'C': params::Cr = std::stod(optarg); break;

			case STRATEGY_FLAG: strategy = optarg; break;
			case PARAMETER_FLAG: param = optarg; break;
			case CREDIT_FLAG: credit = optarg; break;
			case REWARD_FLAG: reward = optarg; break;
			case QUALITY_FLAG: quality = optarg; break;
			case PROBABILITY_FLAG: probability = optarg; break;
			case CONSTRAINT_FLAG: constraint = optarg; break;
			case ID_FLAG: id = optarg; break;
			case ALPHA_FLAG: params::WS_alpha = std::stod(optarg); break;
			case BETA_FLAG: params::AP_beta = std::stod(optarg); break;
			case GAMMA_FLAG: params::PM_AP_pMin_divider = std::stod(optarg); break;
			case POPSIZE_MULTIPLIER_FLAG: params::popsize_multiplier = std::stod(optarg); break;
			case BUDGET_MULTIPLIER_FLAG: BUDGET_MULTIPLIER = std::stoi(optarg); break;
			case INDEPENDENT_RUNS_FLAG: INDEPENDENT_RUNS = std::stoi(optarg); break;
			case COCO_LOG_LEVEL_FLAG: coco_log_level = optarg; break;

			case LOG_ACTIVATIONS_FLAG: 
				params::log_activations = true; 
				if (optarg) params::log_activations_interval = std::stoi(optarg);
				break;
			case LOG_PARAMETERS_FLAG: 
				params::log_parameters = true; 
				if (optarg) params::log_parameters_interval = std::stoi(optarg);
				break;
			case LOG_POSITIONS_FLAG: 
				params::log_positions = true; 
				if (optarg) params::log_positions_interval = std::stoi(optarg);
				break;
			case LOG_DIVERSITY_FLAG: 
				params::log_diversity = true; 
				if (optarg) params::log_diversity_interval = std::stoi(optarg);
				break;
			case LOG_REPAIRS_FLAG: 
				params::log_repairs = true; 
				if (optarg) params::log_repairs_interval = std::stoi(optarg);
				break;
		}
	}

	coco_set_log_level(coco_log_level.c_str());

	DifferentialEvolution de (
		id, 
		{
			.strategy = strategy,
			.constraintHandler = constraint,
			.strategyAdaptationConfig = { 
				.mutation = mutation,
				.crossover = crossover,
				.param = param,
				.credit = credit,
				.reward = reward,
				.quality = quality,
				.probability = probability
			}
		}
	);

	experiment(
		de, 
		"bbob", 
		("dimensions: " + dimensions + " function_indices: " + functions).c_str(), 
		"bbob", 
		("result_folder: " + id).c_str()
	);

	return 0;
}

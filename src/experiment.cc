#include <iostream>
#include <functional>
#include <getopt.h>
#include "coco.h"
#include "differentialevolution.h"
#include "params.h"

static coco_problem_t *PROBLEM;
static int const BUDGET_MULTIPLIER = 1e5;
static int const INDEPENDENT_RUNS = 20;
static std::vector<int> const INSTANCES = {1,2,3,4,5};

#include "default_params.h"

std::string gen_instances(){
	std::string instances = "";
	for (int i : INSTANCES)
		for (int j = 0; j < INDEPENDENT_RUNS; j++)
			instances += std::to_string(i) + ",";
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

int main(int argc, char** argv) {
	coco_set_log_level("warning");

	//params::log_parameters = true;
	//params::log_parameters_interval = 10;
	//params::log_repairs = true;
	//params::log_repairs_interval = 100;
	//params::restart_on_convergence = false;

	// defaults
	std::string 
		strategy    = "A",
		param		= "S",		
		credit		= "SD",
		reward 		= "AA",
		quality 	= "WS",
		probability	= "PM",
		constraint 	= "RS",
		//////////////////////
		suite 		= "bbob",
	  	dimensions 	= "20",
		functions 	= "1-24",
		instances 	= "1-" + std::to_string(INSTANCES.size() * INDEPENDENT_RUNS);

	std::string id = "DE";

	std::vector<std::string>
		mutation = {"BE1", "RA1", "TB2"},
		crossover = {"B", "E"};

	int c;

	while(true){	
		static struct option long_options[] =
		{
			{"strategy", 			required_argument,	0,	1000},
			{"parameter",			required_argument,	0, 	1001},
			{"credit", 				required_argument,	0, 	1002},
			{"reward", 				required_argument,	0, 	1003},
			{"quality", 			required_argument,	0, 	1004},
			{"probability", 		required_argument,	0, 	1005},
			{"constraint", 			required_argument,	0, 	1006},
			{"id", 					required_argument,	0, 	1007},
			{"alpha", 				required_argument,	0, 	1008},
			{"beta", 				required_argument,	0, 	1009},
			{"gamma", 				required_argument,	0, 	1010},
			{"popsize-multiplier",	required_argument,	0, 	1011},
			{"dimensions", 			required_argument,	0, 	'd'},
			{"functions", 			required_argument,	0, 	'f'},
			{"instances", 			required_argument,	0, 	'i'},
			{"mutation", 			required_argument,	0, 	'm'},
			{"crossover", 			required_argument,	0, 	'c'},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		c = getopt_long (argc, argv, "d:f:i:m:c:", long_options, &option_index);

		if (c == -1)
			break;

#define OPT(X,Y) case X: Y = optarg; break;
		switch (c){
			case 0:
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;
			OPT(1000, strategy)
			OPT(1001, param)
			OPT(1002, credit)
			OPT(1003, reward)
			OPT(1004, quality)
			OPT(1005, probability)
			OPT(1006, constraint)
			OPT(1007, id)
			OPT('d', dimensions)
			OPT('f', functions)
			OPT('i', instances)
			case 1008: params::WS_alpha = std::stod(optarg); break;
			case 1009: params::AP_beta = std::stod(optarg); break;
			case 'm': mutation = splitString(optarg); break;
			case 'c': crossover = splitString(optarg); break;
			case 1010: params::PM_AP_pMin_divider = std::stod(optarg); break;
			case 1011: params::popsize_multiplier = std::stod(optarg); break;
		}
	}

	DifferentialEvolution de(
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
		suite.c_str(), 
		("dimensions: " + dimensions + " instance_indices: " + instances + " function_indices: " + functions).c_str(), 
		suite.c_str(), 
		("result_folder: " + id).c_str()
	);

	return 0;
}

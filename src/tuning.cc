#include <iostream>
#include <getopt.h>
#include <iomanip>
#include "coco.h"
#include "differentialevolution.h"
#include "params.h"

static int const BUDGET_MULTIPLIER = 1e4;

#include "default_params.h"

void experiment(DifferentialEvolution& de,
				char const *const suite_name,
				char const *const suite_options,
				char const *const /*observer_name*/,
				char const *const /*observer_options*/) {

	coco_suite_t *suite;
	coco_observer_t *observer;
	suite = coco_suite(suite_name, "instances:1-5" , suite_options);
	observer = coco_observer("no_observer", "");

	// Only run single problem
	coco_problem_t *const PROBLEM = coco_suite_get_next_problem(suite, observer);
	int const dimension = coco_problem_get_dimension(PROBLEM);
	int const popSize = dimension * params::popsize_multiplier;
	size_t const budget = dimension * BUDGET_MULTIPLIER;
	std::string const fid = coco_problem_get_id(PROBLEM);

	do {
		de.run(PROBLEM, budget, popSize);
	} while (!coco_problem_final_target_hit(PROBLEM) && coco_problem_get_evaluations(PROBLEM) < budget);

	std::cout 
		<< std::setprecision(17) 
		<< std::max(coco_problem_get_best_observed_fvalue1(PROBLEM) - coco_problem_get_final_target_fvalue1(PROBLEM),1e-8) 
		<< std::endl;

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

	params::log_activations = false;
	params::log_diversity = false;
	params::log_parameters = false;

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
	 	functions 	= "",
		instances 	= "";

	std::string id = "DE";

	std::vector<std::string>
		mutation = {"BE1", "RA1", "TB2"},
		crossover = {"B", "E"};

	int c;

#define OPT(X,Y) case X: Y = optarg; break;
	while ((c = getopt(argc, argv, "s:P:C:r:q:p:b:d:f:I:i:S:A:B:m:c:M:")) != -1){
		switch (c){
			OPT('s', strategy)
			OPT('P', param)
			OPT('C', credit)
			OPT('r', reward)
			OPT('q', quality)
			OPT('p', probability)
			OPT('b', constraint)
			OPT('d', dimensions)
			OPT('f', functions)
			OPT('I', id)
			OPT('i', instances)
			case 'S': rng.seed(std::stod(optarg)); break;
			case 'A': params::WS_alpha = std::stod(optarg); break;
			case 'B': params::AP_beta = std::stod(optarg); break;
			case 'm': mutation = splitString(optarg); break;
			case 'c': crossover = splitString(optarg); break;
			case 'M': params::PM_AP_pMin_divider = std::stod(optarg); break;
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

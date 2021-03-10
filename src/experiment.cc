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
		std::string const fid = coco_problem_get_id(PROBLEM);

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

#define OPT(X,Y) case X: Y = optarg; break;
	while ((c = getopt(argc, argv, "P:C:r:q:p:b:d:f:i:I:m:c:s:A:B:M:x:")) != -1){
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
			OPT('i', instances)
			OPT('I', id)
			case 'A': params::WS_alpha = std::stod(optarg); break;
			case 'B': params::AP_beta = std::stod(optarg); break;
			case 'm': mutation = splitString(optarg); break;
			case 'c': crossover = splitString(optarg); break;
			case 'M': params::PM_AP_pMin_divider = std::stod(optarg); break;
			case 'x': params::popsize_multiplier = std::stod(optarg); break;
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

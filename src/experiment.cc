#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include "coco.h"
#include "differentialevolution.h"
#include "rng.h"
#include "util.h"
#include <getopt.h>
static coco_problem_t *PROBLEM;
static int const BUDGET_MULTIPLIER = 10000;
static int const POPSIZE_MULTIPLIER = 5;

void experiment(DifferentialEvolution& de,
				char const *const suite_name,
				char const *const suite_options,
				char const *const observer_name,
				char const *const observer_options) {

	coco_suite_t *suite;
	coco_observer_t *observer;
	suite = coco_suite(suite_name, "", suite_options);
	observer = coco_observer(observer_name, observer_options);

	while ((PROBLEM = coco_suite_get_next_problem(suite, observer))) {
		int const dimension = coco_problem_get_dimension(PROBLEM);
		size_t const budget = dimension * BUDGET_MULTIPLIER;
		int const popSize = dimension * POPSIZE_MULTIPLIER;

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
	coco_set_log_level("info");

	std::string 
		param		= "S",		
		credit		= "CO",
		reward 		= "AA",
		quality 	= "WS",
		probability	= "AP",
		constraint 	= "RS",
		suite 		=	"bbob",
	  	dimensions 	= 	"20",
	 	functions 	= 	"1-24",
		instances 	= 	"1-5";

	std::string id = "DE";

	std::vector<std::string>
		mutation = {"BE1"},
		crossover = {"B"};

	int c;

#define OPT(X,Y) case X: Y = optarg; break;
	while ((c = getopt(argc, argv, "P:C:r:q:p:b:d:f:i:I:m:c:")) != -1){
		switch (c){
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
			case 'm': mutation = splitString(optarg); break;
			case 'c': crossover = splitString(optarg); break;
		}
	}

	DifferentialEvolution de({
		{ 
			mutation,
			crossover,
			param,
			credit,
			reward,
			quality,
			probability
		},
	    constraint	
	});

	experiment(
		de, 
		suite.c_str(), 
		("dimensions: " + dimensions + " instance_indices: " + instances + " function_indices: " + functions).c_str(), 
		suite.c_str(), 
		("result_folder: " + id).c_str()
	);

	return 0;
}

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
static coco_problem_t *PROBLEM;
static int const BUDGET_MULTIPLIER = 10000;

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
		int const popSize = dimension * 5;

		do {
			de.run(PROBLEM, budget, popSize);
		} while (!coco_problem_final_target_hit(PROBLEM) && coco_problem_get_evaluations(PROBLEM) < budget);
	}

	coco_observer_free(observer);
	coco_suite_free(suite);
}

int main() {
	coco_set_log_level("info");
	DifferentialEvolution de({

		{ /* -- Strategy self-adaptation configuration -- */
			{"B1", "R1", "T2", "P1"}, 		/* Mutation */ 
			{"B"},							/* Crossover */	
			"S",							/* Parameter self-adaptation */
			"EA", 		 					/* Reward */
			"WS",							/* Quality */
			"PM" 		 					/* Probability */
		},
		"RS"								/* Constraint handling */
	});

	std::string const 
		suite 		=	"bbob",
	  	dimensions 	= 	"20",
	 	functions 	= 	"3",
		instances 	= 	"1";

	experiment(
		de, 
		"bbob", 
		("dimensions: " + dimensions + " instance_indices: " + instances + " function_indices: " + functions).c_str(), 
		"bbob", 
		("result_folder: " + de.getIdString()).c_str()
	);

	return 0;
}

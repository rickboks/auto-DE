#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include "coco.h"
#include "differentialevolution.h"
static coco_problem_t *PROBLEM;
static int const BUDGET_MULTIPLIER = 10000;

/**
 * @param suite_name Name of the suite (e.g. "bbob" or "bbob-biobj").
 * @param suite_options Options of the suite (e.g. "dimensions: 2,3,5,10,20 instance_indices: 1-5").
 * @param observer_name Name of the observer matching with the chosen suite (e.g. "bbob-biobj"
 * when using the "bbob-biobj-ext" suite).
 * @param observer_options Options of the observer (e.g. "result_folder: folder_name")
 */

std::string gen_instance_indices(std::vector<std::string> instances, int independent_runs){
  std::string str = "";
  for (std::string i : instances){
    for (int j = 0; j < independent_runs; j++){
      str += i + ",";
    }
  }
  str.pop_back();
  std::cout << str << std::endl;
  return str;
}

void example_experiment(DifferentialEvolution& de,
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
	while(true){
	  int const evaluations_done = coco_problem_get_evaluations(PROBLEM);
	  int const evaluations_remaining = dimension * BUDGET_MULTIPLIER - evaluations_done;

	  if ((coco_problem_final_target_hit(PROBLEM) 
		&& coco_problem_get_number_of_constraints(PROBLEM) == 0) 
		|| (evaluations_remaining <= 0))
		break; 

	  de.run(PROBLEM, evaluations_remaining, 100);
	}

  }

  coco_observer_free(observer);
  coco_suite_free(suite);
}

int main() {
  coco_set_log_level("info");
  DifferentialEvolution de(DEConfig({"B1"}, {"B"}, "S", "MT"));
  example_experiment(
      de, 
      "bbob", 
      "dimensions: 20 instance_indices: 1" /*+ gen_instance_indices({"1", "2", "3", "4", "5"}, 5)).c_str()*/, 
      "bbob", 
      ("result_folder: " + de.getIdString()).c_str());
  return 0;
}

#include <algorithm>
#include "differentialevolution.h"
#include "params.h"
#include "strategyadaptationmanager.h"
#include "parameteradaptationmanager.h"
#include "constrainthandler.h"
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "util.h"

#define CONVERGENCE_DELTA 1e-9

DifferentialEvolution::DifferentialEvolution(std::string const id, DEConfig const config)
	:id(id), config(config), 
	activationsLogger(params::extra_data_path + "/" + id + ".act"),
	parameterLogger(params::extra_data_path + "/" + id + ".par"){
}

DifferentialEvolution::~DifferentialEvolution(){}

bool DifferentialEvolution::converged(std::vector<Solution*>const& population) const{
	return std::abs(getWorst(population)->getFitness() - getBest(population)->getFitness()) < CONVERGENCE_DELTA;
} 

// Should be called before starting to optimize a problem
void DifferentialEvolution::prepare(coco_problem_t* const problem, int const popSize) {
	this->D = coco_problem_get_dimension(problem);
	this->popSize = popSize;
	this->problem = problem;

	ArrayXd const 
		lowerBound = ArrayXd::Map(coco_problem_get_smallest_values_of_interest(problem), D),
		upperBound = ArrayXd::Map(coco_problem_get_largest_values_of_interest(problem), D);

	// Initialize and evaluate the population
	genomes.resize(popSize);
	for (int i = 0; i < popSize; i++){
		genomes[i] = new Solution(D);
		genomes[i]->randomize(lowerBound, upperBound);
		genomes[i]->evaluate(problem);
	}

	ch = ConstraintHandler::create(config.constraintHandler)(lowerBound, upperBound);
	strategyAdaptationManager = StrategyAdaptationManager::create(config.strategy)(
			config.strategyAdaptationConfig, ch, genomes);

	if (params::log_activations)
		activationsLogger.log(coco_problem_get_id(problem));

	if (params::log_parameters)
		parameterLogger.log(coco_problem_get_id(problem));
}

// Wrapper of prepare -> run -> reset
void DifferentialEvolution::run(coco_problem_t* problem, int const evalBudget, int const popSize){
	prepare(problem, popSize);
	run(evalBudget);
	reset();
}

// Optimize the problem for 'evalBudget' evaluations.
void DifferentialEvolution::run(int const evalBudget){
	ArrayXd Fs(popSize), Crs(popSize);

	std::map<MutationManager*, std::vector<int>> mutationManagers;   // Maps containing the indices that each
	std::map<CrossoverManager*, std::vector<int>> crossoverManagers; // mutation/crossover operator handles.
	ArrayXi recentActivations = ArrayXi::Zero(strategyAdaptationManager->K);

	int iteration = 0;
	while ((int)coco_problem_get_evaluations(problem) < evalBudget
			&& !coco_problem_final_target_hit(problem)
			/*&& !converged(genomes)*/){

		strategyAdaptationManager->next(genomes, mutationManagers, crossoverManagers, Fs, Crs);
		recentActivations += strategyAdaptationManager->getLastActivations();

		if (iteration > 0){
		 	if (params::log_activations && iteration % params::log_activations_interval == 0){
				activationsLogger.log(recentActivations.transpose().format(params::vecFmt));
				recentActivations.setZero();
			}
			if (params::log_parameters && iteration % params::log_parameters_interval == 0){
				parameterLogger.log(Fs.mean(), false); parameterLogger.log(" ", false); parameterLogger.log(Crs.mean());
			}
		}

		// Mutation step
		std::vector<Solution*> donors(popSize);
		for (MutationManager* const m : strategyAdaptationManager->getMutationManagers()){
			if (mutationManagers.count(m)){
				std::vector<int> const indices = mutationManagers[m];
				if (!indices.empty()){
					m->prepare(genomes);
					for (int i : indices)
						 donors[i] = m->mutate(genomes, i, Fs[i]);
				}
			}
		}

		// Crossover step
		std::vector<Solution*> trials(popSize); 
		for (CrossoverManager* const c : strategyAdaptationManager->getCrossoverManagers()){
			for (int i : crossoverManagers[c]){
				trials[i] = c->crossover(genomes[i], donors[i], Crs[i]);
				delete donors[i];

				trials[i]->evaluate(problem);
				ch->penalize(trials[i]); 
			}
		}

		// Update the adaptation manager
		strategyAdaptationManager->update(trials);

		// Selection step
		for (int i = 0; i < popSize; i++){
			if (*trials[i] < *genomes[i]){
				delete genomes[i];
				genomes[i] = trials[i];
			} else {
				delete trials[i];
			}
		}
		iteration++;
	}
}

void DifferentialEvolution::reset(){
	activationsLogger.log(""); // blank line
	parameterLogger.log(""); // blank line
	for (Solution* d : genomes) 
		delete d;
	delete ch;
	delete strategyAdaptationManager;
	genomes.clear();
}

std::string DifferentialEvolution::getIdString() const {
	return id;
}

ArrayXi DifferentialEvolution::getActivations(){
	return strategyAdaptationManager->getTotalActivations();
}

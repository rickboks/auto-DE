#include <algorithm>
#include "differentialevolution.h"
#include "params.h"
#include "strategyadaptationmanager.h"
#include "parameteradaptationmanager.h"
#include "constrainthandler.h"
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "util.h"

DifferentialEvolution::DifferentialEvolution(std::string const id, DEConfig const config)
	:id(id), config(config), 
	activationsLogger(params::extra_data_path + "/" + id + ".act"),
	parameterLogger(params::extra_data_path + "/" + id + ".par"),
	positionsLogger(params::extra_data_path + "/" + id + ".pos"),
	diversityLogger(params::extra_data_path + "/" + id + ".div"),
	repairsLogger(params::extra_data_path + "/" + id + ".rep"){
}

DifferentialEvolution::~DifferentialEvolution(){}

bool DifferentialEvolution::converged(std::vector<Solution*>const& population) const{
	return std::abs(getWorst(population)->getFitness() - getBest(population)->getFitness()) < params::convergence_delta;
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

	if (params::log_activations){
		activationsLogger.log(coco_problem_get_id(problem), false);
		for (std::string const& s : strategyAdaptationManager->getConfigurationIDs())
			activationsLogger.log(" " + s, false);
		activationsLogger.log("");
	}

	if (params::log_parameters)
		parameterLogger.log(coco_problem_get_id(problem));

	if (params::log_diversity)
		diversityLogger.log(coco_problem_get_id(problem));

	if (params::log_repairs)
		repairsLogger.log(coco_problem_get_id(problem));
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
			&& (!params::restart_on_convergence || !converged(genomes))){

		strategyAdaptationManager->next(genomes, mutationManagers, crossoverManagers, Fs, Crs);
		recentActivations += strategyAdaptationManager->getLastActivations();

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

		if (params::log_parameters && iteration % params::log_parameters_interval == 0){
			for (int i = 0; i < popSize; i++){
				parameterLogger.log(int(Fs(i)*100), false);
				parameterLogger.log(":", false);
				parameterLogger.log(int(Crs(i)*100), false);
				if (i != popSize-1)
					parameterLogger.log(" ", false);
			}
			parameterLogger.log("");
		}


		// Selection step
		for (int i = 0; i < popSize; i++){
			if (*trials[i] < *genomes[i]){
				delete genomes[i];
				genomes[i] = trials[i];
			} else {
				delete trials[i];
			}
		}

		/* Logging */
		if (params::log_activations && iteration > 0 && iteration % params::log_activations_interval == 0){
			activationsLogger.log(recentActivations.transpose().format(params::vecFmt));
			recentActivations.setZero();
		}

		if (params::log_diversity && iteration % params::log_diversity_interval == 0)
			diversityLogger.log(strategyAdaptationManager->getDistancesToMeanPosition().mean());

		if (params::log_positions && iteration % params::log_positions_interval == 0){
			for (Solution const* const s : genomes)
				positionsLogger.log(s->X().transpose().format(params::vecFmt));
			positionsLogger.log("");
		}

		if (params::log_repairs && iteration > 0 && iteration % params::log_repairs_interval == 0)
			repairsLogger.log(double(ch->getCorrections()) / (int)coco_problem_get_evaluations(problem));
		/* ----- */
		iteration++;
	}
}

void DifferentialEvolution::reset(){
	if (params::log_activations) activationsLogger.log(""); // blank line
	if (params::log_parameters) parameterLogger.log(""); 
	if (params::log_diversity) diversityLogger.log("");
	if (params::log_repairs) repairsLogger.log("");

	for (Solution* d : genomes) 
		delete d;
	delete ch;
	delete strategyAdaptationManager;
	genomes.clear();
}

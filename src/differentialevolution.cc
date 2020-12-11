#include <algorithm>
#include "differentialevolution.h"
#include "strategyadaptationmanager.h"
#include "util.h"

#define CONVERGENCE_DELTA 1e-10

DifferentialEvolution::DifferentialEvolution(DEConfig const config)
	: config(config){
}

bool DifferentialEvolution::converged(std::vector<Solution*>const& population) const{
	double const maxFitness = (*std::max_element(population.begin(), population.end(), comparePtrs))->getFitness();
	double const minFitness = (*std::min_element(population.begin(), population.end(), comparePtrs))->getFitness();
	return maxFitness - minFitness < CONVERGENCE_DELTA;
}

void DifferentialEvolution::run(coco_problem_t* const problem, int const evalBudget, int const popSize) const {
	int const D = coco_problem_get_dimension(problem);
	std::vector<double> const lowerBound = vectorize(coco_problem_get_smallest_values_of_interest(problem), D);
	std::vector<double> const upperBound = vectorize(coco_problem_get_largest_values_of_interest(problem), D);

	// Initialize and evaluate the population
	std::vector<Solution*> genomes(popSize);
	for (int i = 0; i < popSize; i++){
		genomes[i] = new Solution(D);
		genomes[i]->randomize(lowerBound, upperBound);
		genomes[i]->evaluate(problem);
	}

	ConstraintHandler * const ch = constraintHandlers.at(config.constraintHandler)(lowerBound, upperBound);
	ParameterAdaptationManager* const paramAdaptationManager = parameterAdaptations.at(config.adaptation)(popSize);
	ConfigurationSpace const* const configSpace = new ConfigurationSpace(config.mutation, config.crossover, ch);
	StrategyAdaptationManager* const strategyAdaptationManager = new ConstantStrategyManager(configSpace, popSize);

	std::vector<double> Fs(popSize);
	std::vector<double> Crs(popSize);
	std::map<MutationManager*, std::vector<int>> mutationManagers;   // Maps containing the indices that each
	std::map<CrossoverManager*, std::vector<int>> crossoverManagers; // mutation/crossover operator handles.

	int iteration = 0;
	while ((int)coco_problem_get_evaluations(problem) < evalBudget
			&& !coco_problem_final_target_hit(problem)
			&& !converged(genomes)){

		paramAdaptationManager->nextParameters(Fs, Crs);
		strategyAdaptationManager->nextStrategies(mutationManagers, crossoverManagers);

		// Mutation step
		std::vector<Solution*> donors(popSize);
		for (MutationManager* m : configSpace->mutation){
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
		std::vector<double> trialF(popSize);
		std::vector<Solution*> trials(popSize); 
		for (CrossoverManager* c : configSpace->crossover){
			for (int i : crossoverManagers[c]){
				trials[i] = c->crossover(genomes[i], donors[i], Crs[i]);
				delete donors[i];
				trials[i]->evaluate(problem);
				ch->penalize(trials[i]); 
				trialF[i] = trials[i]->getFitness();
			}
		}

		// Selection step
		std::vector<double> parentF(popSize);
		for (int i = 0; i < popSize; i++){
			parentF[i] = genomes[i]->getFitness();
			if (trialF[i] < parentF[i]){
				delete genomes[i];
				genomes[i] = trials[i];
			} else {
				delete trials[i];
			}
		}

		// Update the adaptation managers
		paramAdaptationManager->update(parentF, trialF);
		strategyAdaptationManager->update(parentF, trialF);
		iteration++;
	}

	// Clean up
	for (Solution* d : genomes) delete d;
	delete configSpace;
	delete strategyAdaptationManager;
	delete paramAdaptationManager;
	delete ch;
	genomes.clear();
}

std::string DifferentialEvolution::getIdString() const {
	return "DE"; // + config.mutation + "_" + config.crossover + "_" + config.adaptation + "_" + config.constraintHandler;
}

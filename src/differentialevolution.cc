#include "differentialevolution.h"
#include "strategyadaptationmanager.h"

DifferentialEvolution::DifferentialEvolution(DEConfig const config)
	: config(config){
}

void DifferentialEvolution::run(std::shared_ptr<IOHprofiler_problem<double> > const problem, 
    		std::shared_ptr<IOHprofiler_csv_logger> const iohLogger, 
    		int const evalBudget, int const popSize) const {

	int const D = problem->IOHprofiler_get_number_of_variables();
	std::vector<double> const lowerBound = problem->IOHprofiler_get_lowerbound();
	std::vector<double> const upperBound = problem->IOHprofiler_get_upperbound();

	std::vector<Solution*> genomes(popSize);

	for (int i = 0; i < popSize; i++){
		genomes[i] = new Solution(D);
		genomes[i]->randomize(lowerBound, upperBound);
		genomes[i]->evaluate(problem, iohLogger);
	}

	ConstraintHandler * const ch = constraintHandlers.at(config.constraintHandler)(lowerBound, upperBound);
	ParameterAdaptationManager* const paramAdaptationManager = deAdaptations.at(config.adaptation)(popSize);
	ConfigurationSpace const* const configSpace = new ConfigurationSpace(config.mutation, config.crossover, D, ch);
	StrategyAdaptationManager* const strategyAdaptationManager = new ConstantStrategyManager(configSpace, popSize);

	std::vector<double> Fs(popSize);
	std::vector<double> Crs(popSize);

	int iteration = 0;

	std::map<MutationManager*, std::vector<int>> mutationManagers;
	std::map<CrossoverManager*, std::vector<int>> crossoverManagers; 

	while (problem->IOHprofiler_get_evaluations() < evalBudget && !problem->IOHprofiler_hit_optimal()){
		paramAdaptationManager->nextParameters(Fs, Crs);
		strategyAdaptationManager->nextStrategies(mutationManagers, crossoverManagers);

		// Mutation step
		std::vector<Solution*> donors(popSize);
		for (MutationManager* m : configSpace->mutation){
			m->preMutation(genomes);
			for (int i: mutationManagers[m])
				 donors[i] = m->mutate(genomes, i, Fs[i]);
		}

		// Crossover step
		std::vector<double> trialF(popSize);
		std::vector<Solution*> trials(popSize); 
		for (CrossoverManager* c : configSpace->crossover){
			for (int i : crossoverManagers[c]){
				trials[i] = c->crossover(genomes[i], donors[i], Crs[i]);
				delete donors[i];
				trials[i]->evaluate(problem, iohLogger);
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

		// Housekeeping
		paramAdaptationManager->update(parentF, trialF);
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

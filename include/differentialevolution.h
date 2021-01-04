#pragma once
#include "coco.h"
#include <vector>
#include <string>

template <typename T>
class IOHprofiler_problem;
class IOHprofiler_csv_logger;
class ParameterAdaptationManager;
class StrategyAdaptationManager;
class Solution;
class ConfigurationSpace;
class ConstraintHandler;

struct DEConfig {
	DEConfig(std::vector<std::string> mutation, std::vector<std::string> crossover, 
			std::string const adaptation, std::string const constraintHandler)
			: mutation(mutation), crossover(crossover), adaptation(adaptation), constraintHandler(constraintHandler){}

	std::vector<std::string> const mutation, crossover;
	std::string const adaptation, constraintHandler;
};

class DifferentialEvolution {
	private:
		DEConfig const config;
		std::vector<Solution*> genomes;
		ConstraintHandler* ch;
		ParameterAdaptationManager* paramAdaptationManager;
		ConfigurationSpace const* configSpace;
		StrategyAdaptationManager* strategyAdaptationManager;
		int popSize;
		int D;
		coco_problem_t* problem;
	public:
		DifferentialEvolution(DEConfig const config);
		virtual ~DifferentialEvolution();
		void run(int const evalBudget);
		void run(coco_problem_t* problem, int const evalBudget, int const popSize);
		void prepare(coco_problem_t* problem, int const popSize);
		void reset();
		bool converged(std::vector<Solution*>const& population) const;
		std::string getIdString() const;
};

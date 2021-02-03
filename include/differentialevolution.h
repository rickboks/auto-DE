#pragma once
#include "coco.h"
#include "strategyadaptationmanager.h"
#include <vector>
#include <string>

template <typename T>
class IOHprofiler_problem;
class IOHprofiler_csv_logger;
class ParameterAdaptationManager;
class StrategyAdaptationManager;
class Solution;
class ConstraintHandler;

struct DEConfig {
	DEConfig(StrategyAdaptationConfiguration const strategyAdaptationConfig,
			std::string const constraintHandler)
			: constraintHandler(constraintHandler), strategyAdaptationConfig(strategyAdaptationConfig){}

	std::string const constraintHandler;
	StrategyAdaptationConfiguration const strategyAdaptationConfig;
};

class DifferentialEvolution {
	private:
		DEConfig const config;
		std::vector<Solution*> genomes;
		ConstraintHandler* ch;
		StrategyAdaptationManager* strategyAdaptationManager;
		int popSize;
		int D;
		coco_problem_t* problem;
	public:
		std::string const id;
		DifferentialEvolution(std::string const id, DEConfig const config);
		virtual ~DifferentialEvolution();
		void run(int const evalBudget);
		void run(coco_problem_t* problem, int const evalBudget, int const popSize);
		void prepare(coco_problem_t* problem, int const popSize);
		void reset();
		bool converged(std::vector<Solution*>const& population) const;
		std::string getIdString() const;
};

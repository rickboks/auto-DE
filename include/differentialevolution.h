#pragma once
#include <vector>
#include <string>
#include "coco.h"
#include "strategyadaptationmanager.h"
#include "logger.h"

class ParameterAdaptationManager;
class StrategyAdaptationManager;
class Solution;
class ConstraintHandler;

struct DEConfig {
	std::string const strategy, constraintHandler;
	StrategyAdaptationConfiguration const strategyAdaptationConfig;
};

class DifferentialEvolution {
	public:
		std::string const id;
		DifferentialEvolution(std::string const id, DEConfig const config);
		virtual ~DifferentialEvolution();
		void run(int const evalBudget);
		void run(coco_problem_t* problem, int const evalBudget, int const popSize);
		void prepare(coco_problem_t* problem, int const popSize);
		void reset();
		bool converged(std::vector<Solution*>const& population) const;
	private:
		DEConfig const config;
		std::vector<Solution*> genomes;
		ConstraintHandler* ch;
		StrategyAdaptationManager* strategyAdaptationManager;
		int popSize;
		int D;
		coco_problem_t* problem;
		Logger activationsLogger;
		Logger parameterLogger;
		Logger positionsLogger;
		Logger diversityLogger;
		Logger repairsLogger;
};

#pragma once
#include <vector>
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "configurationspace.h"

class StrategyAdaptationManager {
	public:
		StrategyAdaptationManager(ConfigurationSpace const* const configSpace, int const popSize);
		virtual ~StrategyAdaptationManager(){};
		virtual void nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover)=0;
	protected:
		ConfigurationSpace const* const configSpace;
		int const popSize;
};

class ConstantStrategyManager : public StrategyAdaptationManager {
	public:
		ConstantStrategyManager(ConfigurationSpace const* const configSpace, int const popSize);
		void nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover);
};

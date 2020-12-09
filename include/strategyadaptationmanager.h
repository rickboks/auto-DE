#pragma once
#include <vector>
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "configurationspace.h"

class StrategyAdaptationManager {
	public:
		StrategyAdaptationManager(ConfigurationSpace const configSpace);
		virtual ~StrategyAdaptationManager(){};
		virtual void nextStrategies(std::vector<MutationManager*>& mutation, std::vector<CrossoverManager*>& crossover)=0;
	protected:
		ConfigurationSpace const configSpace;
};

class ConstantStrategyManager : public StrategyAdaptationManager {
	public:
		ConstantStrategyManager(ConfigurationSpace const configSpace);
		void nextStrategies(std::vector<MutationManager*>& mutation, std::vector<CrossoverManager*>& crossover);
	private:
};

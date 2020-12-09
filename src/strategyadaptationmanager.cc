#include <exception>
#include <stdexcept>
#include "strategyadaptationmanager.h"

StrategyAdaptationManager::StrategyAdaptationManager(ConfigurationSpace const configSpace)
	:configSpace(configSpace){
}

ConstantStrategyManager::ConstantStrategyManager(ConfigurationSpace const configSpace)
	:StrategyAdaptationManager(configSpace){

	if (configSpace.mutation.size() != 1 || configSpace.crossover.size() != 1){
		throw std::invalid_argument("ConstantStrategyManager needs exactly 1 mutation and 1 crossover type");
	}
}

void ConstantStrategyManager::nextStrategies(std::vector<MutationManager*>& mutation, 
		std::vector<CrossoverManager*>& crossover){
	std::fill(mutation.begin(), mutation.end(), *configSpace.mutation.begin());
	std::fill(crossover.begin(), crossover.end(), *configSpace.crossover.begin());
}

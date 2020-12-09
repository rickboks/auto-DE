#include <exception>
#include "strategyadaptationmanager.h"

StrategyAdaptationManager::StrategyAdaptationManager(ConfigurationSpace const* const configSpace, int const popSize)
	:configSpace(configSpace), popSize(popSize){
}

ConstantStrategyManager::ConstantStrategyManager(ConfigurationSpace const* const configSpace, int const popSize)
	:StrategyAdaptationManager(configSpace, popSize){

	if (configSpace->mutation.size() != 1 || configSpace->crossover.size() != 1){
		throw std::invalid_argument("ConstantStrategyManager needs exactly 1 mutation and 1 crossover type");
	}
}

void ConstantStrategyManager::nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
		std::map<CrossoverManager*, std::vector<int>>& crossover){

	std::vector<int> key(popSize);
	std::iota(key.begin(), key.end(), 0); // All indices [0,M-1]
	mutation[*configSpace->mutation.begin()] = key;
	crossover[*configSpace->crossover.begin()] = key;
}

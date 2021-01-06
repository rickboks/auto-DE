#include <stdexcept>
#include <exception>
#include "strategyadaptationmanager.h"
#include "util.h"

StrategyAdaptationManager::StrategyAdaptationManager(ConfigurationSpace const* const configSpace, int const popSize)
	:configSpace(configSpace), popSize(popSize){

	configs = configSpace->getConfigurations();
}

ConstantStrategyManager::ConstantStrategyManager(ConfigurationSpace const* const configSpace, int const popSize)
	:StrategyAdaptationManager(configSpace, popSize){

	if (configs.size() > 1)
		throw std::invalid_argument("ConstantStrategyManager needs exactly 1 configuration");
}

void ConstantStrategyManager::nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
		std::map<CrossoverManager*, std::vector<int>>& crossover){
	std::vector<int> key(popSize);
	std::iota(key.begin(), key.end(), 0); // All indices [0,M-1]

	mutation[std::get<0>(configs.front())] = key;
	crossover[std::get<1>(configs.front())] = key;
}

AdapSSManager::AdapSSManager(ConfigurationSpace const* const configSpace, int const popSize)
	: StrategyAdaptationManager(configSpace, popSize), alpha(.3), beta(.8), pMin(.05), 
	p(configs.size(), 1./configs.size()), q(configs.size(), 0), previousStrategies(popSize), indices(range(popSize)){
}

void AdapSSManager::nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
		std::map<CrossoverManager*, std::vector<int>>& crossover){
	mutation.clear(); 
	crossover.clear();

	std::vector<int> assignment = rouletteSelect<int>(indices, p, popSize, true);
	previousStrategies = assignment;
	
	for (int i = 0; i < popSize; i++){
		auto [ m, c ] = configs[assignment[i]];
		if (!mutation.count(m)) mutation[m] = {};
		if (!crossover.count(c)) crossover[c] = {};
		mutation[m].push_back(i); crossover[c].push_back(i);
	}
}

void AdapSSManager::update(std::vector<double>const parentF, std::vector<double>const trialF){
	std::vector<std::vector<double>> deltas;
				
}

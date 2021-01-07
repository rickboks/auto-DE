#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <exception>
#include "strategyadaptationmanager.h"
#include "util.h"
#include "rewardmanager.h"

StrategyAdaptationManager::StrategyAdaptationManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler * const ch, int const popSize)
	:config(config), popSize(popSize){

	for (std::string m : config.mutation)
		mutationManagers.push_back(mutations.at(m)(ch));
	for (std::string c : config.crossover)
		crossoverManagers.push_back(crossovers.at(c)());
	for (auto m : mutationManagers)
		for (auto c : crossoverManagers)
			configurations.push_back({m,c});

	K = configurations.size();
}

std::vector<MutationManager*> StrategyAdaptationManager::getMutationManagers() const{
	return mutationManagers;
}

std::vector<CrossoverManager*> StrategyAdaptationManager::getCrossoverManagers() const{
	return crossoverManagers;
}

StrategyAdaptationManager::~StrategyAdaptationManager(){
	for (auto m : mutationManagers)
		delete m;
	for (auto c : crossoverManagers)
		delete c;
}

ConstantStrategyManager::ConstantStrategyManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler*const ch, int const popSize)
	:StrategyAdaptationManager(config, ch, popSize){

	if (configurations.size() > 1)
		throw std::invalid_argument("ConstantStrategyManager needs exactly 1 configuration");
}

void ConstantStrategyManager::nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
		std::map<CrossoverManager*, std::vector<int>>& crossover){
	std::vector<int> key(popSize);
	std::iota(key.begin(), key.end(), 0); // All indices [0,M-1]

	mutation[std::get<0>(configurations.front())] = key;
	crossover[std::get<1>(configurations.front())] = key;
}

AdaptiveStrategyManager::AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler*const ch, int const popSize)
	: StrategyAdaptationManager(config, ch, popSize), rewardManager(rewardManagers.at(config.reward)()),
			alpha(.4), beta(.6), pMin(.2/K), pMax(pMin + 1. - K * pMin), p(K, 1./K), q(K, 0.), 
	previousStrategies(popSize), indices(range(popSize)){
}

AdaptiveStrategyManager::~AdaptiveStrategyManager(){
	delete rewardManager;
}

void AdaptiveStrategyManager::nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
		std::map<CrossoverManager*, std::vector<int>>& crossover){
	mutation.clear(); 
	crossover.clear();

	std::vector<int> assignment = rouletteSelect<int>(indices, p, popSize, true);
	previousStrategies = assignment;

	for (int i = 0; i < popSize; i++){
		auto [m, c] = configurations[assignment[i]];
		if (!mutation.count(m)) mutation[m] = {};
		if (!crossover.count(c)) crossover[c] = {};
		mutation[m].push_back(i); 
		crossover[c].push_back(i);
	}
}

void AdaptiveStrategyManager::update(std::vector<double>const& parentF, std::vector<double>const& trialF){
	std::vector<std::vector<double>> deltas(K, std::vector<double>());
	double const bestF = *std::min_element(trialF.begin(), trialF.end());

	for (int i = 0; i < popSize; i++){
		double const delta = (trialF <= parentF ? (bestF / trialF[i]) * std::abs(parentF[i] - trialF[i]) : 0.);
		deltas[previousStrategies[i]].push_back(delta);
	}

	std::vector<double> const r = rewardManager->getReward(deltas);
	updateQuality(r);
	updateProbability();
}

void AdaptiveStrategyManager::updateQuality(std::vector<double>const& r){
	for (int i = 0; i < K; i++)
		q[i] += alpha * (r[i] - q[i]);
}

// Adaptive Pursuit
void AdaptiveStrategyManager::updateProbability(){
	int const bestIdx = std::distance(q.begin(), std::max_element(q.begin(), q.end()));
	for (int i = 0; i < K; i++){
		if (i == bestIdx)
			p[i] += beta * (pMax - p[i]);
		else 
			p[i] += beta * (pMin - p[i]);
	}
}

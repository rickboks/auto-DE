#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <exception>
#include "strategyadaptationmanager.h"
#include "util.h"
#include "rewardmanager.h"

StrategyAdaptationManager::StrategyAdaptationManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler * const ch, std::vector<Solution*>const& population)
	:config(config), popSize(population.size()), K(config.crossover.size() * config.mutation.size()){
	for (std::string m : config.mutation)
		mutationManagers.push_back(mutations.at(m)(ch));
	for (std::string c : config.crossover)
		crossoverManagers.push_back(crossovers.at(c)());
	for (auto m : mutationManagers)
		for (auto c : crossoverManagers)
			configurations.push_back({m,c});
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
		ConstraintHandler*const ch, std::vector<Solution*>const& population)
	:StrategyAdaptationManager(config, ch, population){

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
		ConstraintHandler*const ch, std::vector<Solution*>const& population)
	: StrategyAdaptationManager(config, ch, population), rewardManager(rewardManagers.at(config.reward)()),
	probabilityManager(probabilityManagers.at(config.probability)(K)), alpha(.4), p(K, 1./K), q(K, 0.), 
	previousStrategies(popSize){
	
	previousF.reserve(popSize);
	for (Solution* s : population)
		previousF.push_back(s->getFitness());
}

AdaptiveStrategyManager::~AdaptiveStrategyManager(){
	delete rewardManager;
	delete probabilityManager;
}

void AdaptiveStrategyManager::nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
		std::map<CrossoverManager*, std::vector<int>>& crossover){
	mutation.clear(); crossover.clear();
	std::vector<int> indices = range(K); // Range [0,K-1]
	std::vector<int> const assignment = rouletteSelect<int>(indices, p, popSize, true);
	previousStrategies = assignment;

	for (int i = 0; i < popSize; i++){
		auto const [m, c] = configurations[assignment[i]];
		if (!mutation.count(m)) mutation[m] = {};
		if (!crossover.count(c)) crossover[c] = {};
		mutation[m].push_back(i); 
		crossover[c].push_back(i);
	}
}

void AdaptiveStrategyManager::update(std::vector<Solution*>const& population){
	std::vector<std::vector<double>> deltas(K, std::vector<double>());

	std::vector<double> trialF(popSize);
	for (int i = 0; i < popSize; i++)
		trialF[i] = population[i]->getFitness();

	double const bestF = *std::min_element(trialF.begin(), trialF.end());

	for (int i = 0; i < popSize; i++){
		double const delta = (trialF <= previousF ? (bestF / trialF[i]) * std::abs(previousF[i] - trialF[i]) : 0.);
		deltas[previousStrategies[i]].push_back(delta);
	}

	updateQuality(rewardManager->getReward(deltas));
	probabilityManager->updateProbability(q, p);
	previousF = trialF;
}

void AdaptiveStrategyManager::updateQuality(std::vector<double>const r){
	for (int i = 0; i < K; i++)
		q[i] += alpha * (r[i] - q[i]);
}

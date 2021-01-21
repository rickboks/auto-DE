#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <exception>
#include "strategyadaptationmanager.h"
#include "rewardmanager.h"
#include "util.h"

StrategyAdaptationManager::StrategyAdaptationManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler * const ch, std::vector<Solution*>const& population)
	:config(config), popSize(population.size()), K(config.crossover.size() * config.mutation.size()), 
	D(population[0]->D), parameterAdaptationManager(parameterAdaptations.at(config.param)(population,K)){

	for (std::string m : config.mutation)
		mutationManagers.push_back(mutations.at(m)(ch));
	for (std::string c : config.crossover)
		crossoverManagers.push_back(crossovers.at(c)());
	for (auto m : mutationManagers)
		for (auto c : crossoverManagers)
			configurations.push_back({m,c});
}

void StrategyAdaptationManager::update(std::vector<Solution*>const& population){
	std::vector<double> trialF(popSize);
	for (int i = 0; i < popSize; i++)
		trialF[i] = population[i]->getFitness();
	parameterAdaptationManager->update(trialF);
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
	delete parameterAdaptationManager;
}

ConstantStrategyManager::ConstantStrategyManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler*const ch, std::vector<Solution*>const& population)
	:StrategyAdaptationManager(config, ch, population){

	if (configurations.size() > 1)
		throw std::invalid_argument("ConstantStrategyManager needs exactly 1 configuration");
}

void ConstantStrategyManager::next(std::map<MutationManager*, std::vector<int>>& mutation, std::map<CrossoverManager*, 
		std::vector<int>>& crossover, std::vector<double>& Fs, std::vector<double>& Crs){

	std::vector<int> key(popSize);
	std::iota(key.begin(), key.end(), 0); // All indices [0,M-1]
	mutation[std::get<0>(configurations.front())] = key;
	crossover[std::get<1>(configurations.front())] = key;

	std::vector<int> assignment(popSize, 0);
	parameterAdaptationManager->nextParameters(Fs, Crs, assignment);
}

AdaptiveStrategyManager::AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler*const ch, std::vector<Solution*>const& population)
	: StrategyAdaptationManager(config, ch, population), 
	rewardManager(rewardManagers.at(config.reward)(K)),
	probabilityManager(probabilityManagers.at(config.probability)(K)), 
	alpha(.8), p(K, 1./K), q(K, 0.), previousStrategies(popSize), previousFitness(popSize){
	
	std::transform(population.begin(), population.end(), previousFitness.begin(), 
			[](Solution const* const s){return s->getFitness();});
	previousDiversity = getDiversity(population);
}

AdaptiveStrategyManager::~AdaptiveStrategyManager(){
	delete rewardManager;
	delete probabilityManager;
}

void AdaptiveStrategyManager::next(std::map<MutationManager*, std::vector<int>>& mutation, std::map<CrossoverManager*, 
		std::vector<int>>& crossover, std::vector<double>& Fs, std::vector<double>& Crs){
	mutation.clear(); crossover.clear();
	std::vector<int> indices = range(K); // Range [0,K-1]
	std::vector<int> const assignment = rouletteSelect(indices, p, popSize, true);
	previousStrategies = assignment;

	for (int i = 0; i < popSize; i++){
		auto const [m, c] = configurations[assignment[i]];
		if (!mutation.count(m)) mutation[m] = {};
		if (!crossover.count(c)) crossover[c] = {};
		mutation[m].push_back(i); 
		crossover[c].push_back(i);
	}

	parameterAdaptationManager->nextParameters(Fs, Crs, assignment);
}

void AdaptiveStrategyManager::update(std::vector<Solution*>const& population){
	std::vector<double> improvement(popSize);
	std::vector<double> trialF(popSize);

	for (int i = 0; i < popSize; i++){
		trialF[i] = population[i]->getFitness();
		// No negative values; there is no difference between a large fitness deterioration and a small one
		improvement[i] = (trialF[i] < previousFitness[i] ? previousFitness[i] - trialF[i] : 0.);
	}

	std::vector<double> const currentDiversity = getDiversity(population); // distance to mean for each individual
	double const stdDev = mean(currentDiversity); // average distance to mean of the entire population
	std::vector<double> diversityFactor = divide(subtract(currentDiversity, previousDiversity), stdDev);
	
	// Don't punish exploitation, only reward exploration
	std::transform(diversityFactor.begin(), diversityFactor.end(), diversityFactor.begin(), 
			[](double const& x){return std::max(1.+x, 1.);}); 

	// pairwise multiply 
	std::transform(improvement.begin(), improvement.end(), diversityFactor.begin(), 
			improvement.begin(), [](double const& x, double const& y){return x*y;}); 

	std::vector<double> const reward = rewardManager->getReward(improvement, previousStrategies);

	updateQuality(reward);
	probabilityManager->updateProbability(q, p);
	parameterAdaptationManager->update(improvement);
	previousFitness = trialF;
	previousDiversity = currentDiversity;
}

void AdaptiveStrategyManager::updateQuality(std::vector<double>const& r){
	for (int i = 0; i < K; i++)
		q[i] += alpha * (r[i] - q[i]);
}

std::vector<double> AdaptiveStrategyManager::getDiversity(std::vector<Solution*>const& population) const{
	std::vector<double> distances(popSize, 0.); //  Deviation for each population member
	std::vector<double> mean(D, 0.); // Mean position of the entire population

	for (Solution* s : population) 
		mean = add(mean, s->getX());
	std::transform(mean.begin(), mean.end(), mean.begin(), 
			[popSize = population.size()](double const& x){return x/popSize;});

	for (unsigned int i = 0; i < population.size(); i++){ 
		std::vector<double> const pos = population[i]->getX();
		for (int k = 0; k < D; k++)
			distances[i] += pow(pos[k] - mean[k], 2.);
		distances[i] = sqrt(distances[i]);
	}
	return distances; 
}

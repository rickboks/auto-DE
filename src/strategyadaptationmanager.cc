#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <exception>
#include "strategyadaptationmanager.h"
#include "util.h"
#include "fitnessrewardmanager.h"

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

void ConstantStrategyManager::next(std::vector<Solution*>const& /*population*/, std::map<MutationManager*, 
		std::vector<int>>& mutation, std::map<CrossoverManager*, std::vector<int>>& crossover, 
		std::vector<double>& Fs, std::vector<double>& Crs){

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
	fitnessRewardManager(fitnessRewardManagers.at(config.fitnessReward)(K)),
	probabilityManager(probabilityManagers.at(config.probability)(K)), alpha(.8), p(K, 1./K), q(K, 0.), 
	previousStrategies(popSize){
	
	previousFitness.reserve(popSize);
	for (Solution* s : population)
		previousFitness.push_back(s->getFitness());

}

AdaptiveStrategyManager::~AdaptiveStrategyManager(){
	delete fitnessRewardManager;
	delete probabilityManager;
}

void AdaptiveStrategyManager::next(std::vector<Solution*>const& population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, 
				std::vector<double>& Fs, std::vector<double>& Crs){
	mutation.clear(); crossover.clear();
	std::vector<int> indices = range(K); // Range [0,K-1]
	std::vector<int> const assignment = rouletteSelect(indices, p, popSize, true);
	previousStrategies = assignment;

	diversityBefore = getDiversity(population, assignment);

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
	std::vector<double> fitnessImprovement(popSize);
	std::vector<double> trialF(popSize);

	for (int i = 0; i < popSize; i++){
		trialF[i] = population[i]->getFitness();
		// No negative values; there is no difference between a large decrease in fitness and a small one
		fitnessImprovement[i] = (trialF[i] < previousFitness[i] ? previousFitness[i] - trialF[i] : 0.);
	}

	std::vector<double> const diversityReward = 
		normalize(subtract(getDiversity(population, previousStrategies), diversityBefore)); 
	std::vector<double> const fitnessReward = fitnessRewardManager->getReward(fitnessImprovement, previousStrategies);

	updateQuality(fitnessReward);
	probabilityManager->updateProbability(q, p);
	parameterAdaptationManager->update(trialF);
	previousFitness = trialF;
}

void AdaptiveStrategyManager::updateQuality(std::vector<double>const& r){
	for (int i = 0; i < K; i++)
		q[i] += alpha * (r[i] - q[i]);
}

std::vector<double> AdaptiveStrategyManager::getDiversity(std::vector<Solution*>const& population, 
		std::vector<int>const& assignment) const{

	std::vector<std::vector<std::vector<double>>> positions(K); // For each configuration, a vector of positions
	std::vector<double> stdDev(K, 0.); // standard deviation for each config
	std::vector<double> mean(D, 0.); // Mean position of the *entire* population

	for (int i = 0; i < popSize; i++){
		std::vector<double> const position = population[i]->getX();
		positions[assignment[i]].push_back(position); // Get positions per configuration
		mean = add(mean, position);
	}
	std::transform(mean.begin(), mean.end(), mean.begin(), 
			[popSize = population.size()](double const& x){return x/popSize;});

	for (int i = 0; i < K; i++){
		if (!positions[i].empty()){ // Calculate the standard deviation
			for (unsigned int j = 0; j < positions[i].size(); j++){ 
				double dist = 0; // Calulate distance of one position to mean
				for (int k = 0; k < D; k++)
					dist += pow(positions[i][j][k] - mean[k], 2.);
				stdDev[i] += sqrt(dist);
			}
			stdDev[i] /= positions[i].size(); // average over all positions of this config
		}
	}
	return stdDev; 
}

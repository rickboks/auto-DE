#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <exception>
#include "strategyadaptationmanager.h"
#include "rewardmanager.h"
#include "util.h"
#include "qualitymanager.h"
#include "creditmanager.h"
#include "probabilitymanager.h"
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "parameteradaptationmanager.h"

std::function<StrategyAdaptationManager* (StrategyAdaptationConfiguration const, ConstraintHandler *const, 
		std::vector<Solution*>const&)> StrategyAdaptationManager::create(std::string const id){
#define ALIAS(X, Y) if (id == X) return [](StrategyAdaptationConfiguration const s, ConstraintHandler *const c, \
	std::vector<Solution*>const& p){return new Y(s,c,p);};
	ALIAS("A", AdaptiveStrategyManager)
	ALIAS("R", RandomStrategyManager)
	ALIAS("C", ConstantStrategyManager)
	throw std::invalid_argument("no such StrategyAdaptationManager: " + id);
}

StrategyAdaptationManager::StrategyAdaptationManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler * const ch, std::vector<Solution*>const& population)
	: K(config.crossover.size() * config.mutation.size()), config(config), popSize(population.size()),  
	D(population[0]->D), parameterAdaptationManager(ParameterAdaptationManager::create(config.param)(popSize,K)), 
	previousStrategies(popSize){

	for (std::string m : config.mutation)
		mutationManagers.push_back(MutationManager::create(m)(ch));
	for (std::string c : config.crossover)
		crossoverManagers.push_back(CrossoverManager::create(c)());
	for (auto m : mutationManagers)
		for (auto c : crossoverManagers)
			configurations.push_back({m,c});

	for (std::string m : config.mutation)
		for (std::string c : config.crossover)
			configurationIDs.push_back(m + "_" + c);
}

std::vector<std::string> StrategyAdaptationManager::getConfigurationIDs(){
	return configurationIDs;
}

void StrategyAdaptationManager::assign(std::map<MutationManager*, std::vector<int>>& mutation, 
		std::map<CrossoverManager*, std::vector<int>>& crossover, std::vector<int> const& assignment){
	mutation.clear(); 
	crossover.clear();
	for (int i = 0; i < popSize; i++){
		MutationManager* const m = std::get<0>(configurations[assignment[i]]);
		CrossoverManager* const c = std::get<1>(configurations[assignment[i]]);
		if (!mutation.count(m)) mutation[m] = {};
		if (!crossover.count(c)) crossover[c] = {};
		mutation[m].push_back(i); 
		crossover[c].push_back(i);
	}
}

std::vector<MutationManager*> StrategyAdaptationManager::getMutationManagers() const{
	return mutationManagers;
}

std::vector<CrossoverManager*> StrategyAdaptationManager::getCrossoverManagers() const{
	return crossoverManagers;
}

ArrayXi StrategyAdaptationManager::getLastActivations() const{
	ArrayXi activations = ArrayXi::Zero(K);
	for (int i : previousStrategies)
		activations(i)++;
	return activations;
}

ArrayXd StrategyAdaptationManager::getDistancesToMeanPosition() const {
	throw "The selected StrategyAdaptationManager does not keep track of diversity";
}

StrategyAdaptationManager::~StrategyAdaptationManager(){
	for (auto m : mutationManagers)
		delete m;
	for (auto c : crossoverManagers)
		delete c;
	delete parameterAdaptationManager;
}

AdaptiveStrategyManager::AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler*const ch, std::vector<Solution*>const& population)
	: StrategyAdaptationManager(config, ch, population), 
	creditManager(CreditManager::create(config.credit)()),
	rewardManager(RewardManager::create(config.reward)(K)),
	qualityManager(QualityManager::create(config.quality)(K)),
	probabilityManager(ProbabilityManager::create(config.probability)(K)), 
	p(ArrayXd::Constant(K, 1./K)), q(ArrayXd::Constant(K, 1.)), used(K){
}

AdaptiveStrategyManager::~AdaptiveStrategyManager(){
	delete creditManager;
	delete rewardManager;
	delete qualityManager;
	delete probabilityManager;
}

void AdaptiveStrategyManager::next(std::vector<Solution*>const& population, std::map<MutationManager*, 
		std::vector<int>>& mutation, std::map<CrossoverManager*, std::vector<int>>& crossover, 
		ArrayXd& Fs, ArrayXd& Crs){
	previousMean = getMean(population);
	previousDistances = getDistances(population, previousMean);

	previousStrategies = //Roulette with replacement
		rouletteSelect(range(K), std::vector<double>(p.begin(), p.end()), popSize, true); 

	// Update used strategies
	std::fill(used.begin(), used.end(), false);
	for (int i : previousStrategies) 
		used[i] = 1;

	previousFitness = ArrayXd::NullaryExpr(popSize, [population](Eigen::Index const i){
		return population[i]->getFitness();
	});

	assign(mutation, crossover, previousStrategies);

	parameterAdaptationManager->nextParameters(Fs, Crs, 
			ArrayXi::Map(previousStrategies.data(), previousStrategies.size()));
}

void AdaptiveStrategyManager::update(std::vector<Solution*>const& trials){
	// Fitness improvements. Deteriorations are set to 0.
	ArrayXd const fitnessDeltas = ArrayXd::NullaryExpr(popSize, [trials, this](Eigen::Index const i){
		return previousFitness[i] - trials[i]->getFitness();
	});

	ArrayXd const currentDistances = getDistances(trials, previousMean);

	ArrayXd const credit = creditManager->getCredit(fitnessDeltas, previousDistances, currentDistances);

	ArrayXd const r = rewardManager->getReward(
			credit, ArrayXi::Map(previousStrategies.data(), previousStrategies.size()));

	qualityManager->updateQuality(q, r, used);

	probabilityManager->updateProbability(p, q);
	parameterAdaptationManager->update(credit);
}

ArrayXd AdaptiveStrategyManager::getMean(std::vector<Solution*>const& population) const{
	ArrayXXd pop(D, popSize);
	for (int i = 0; i < popSize; i++)
		pop.col(i) = population[i]->X();
	return pop.rowwise().mean();
}

ArrayXd AdaptiveStrategyManager::getDistances(std::vector<Solution*>const& population, // Distances w.r.t. mean
		ArrayXd const& mean) const {
	return ArrayXd::NullaryExpr(popSize, [population, mean](Eigen::Index const i){
			return distance(population[i]->X(), mean);
	}); 
}

ArrayXd AdaptiveStrategyManager::getDistancesToMeanPosition() const {
	return previousDistances;
}

RandomStrategyManager::RandomStrategyManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler*const ch, std::vector<Solution*>const& population)
	: StrategyAdaptationManager(config, ch, population){
}

void RandomStrategyManager::next(std::vector<Solution*>const& population, std::map<MutationManager*, 
		std::vector<int>>& mutation, std::map<CrossoverManager*, std::vector<int>>& crossover, 
		ArrayXd& Fs, ArrayXd& Crs){

	for (int i = 0; i < popSize; i++)
		previousStrategies[i] = rng.randInt(0, K-1); // uniformly random allocation

	previousFitness = ArrayXd::NullaryExpr(popSize, [population](Eigen::Index const i){
		return population[i]->getFitness();
	});

	assign(mutation, crossover, previousStrategies);

	parameterAdaptationManager->nextParameters(Fs, Crs, 
			ArrayXi::Map(previousStrategies.data(), previousStrategies.size()));
}

void RandomStrategyManager::update(std::vector<Solution*>const& trials){
	// Fitness improvements. Deteriorations are set to 0.
	ArrayXd const credit = ArrayXd::NullaryExpr(popSize, [trials, this](Eigen::Index const i){
		return previousFitness[i] - trials[i]->getFitness();
	}).max(0);

	parameterAdaptationManager->update(credit);
}

ConstantStrategyManager::ConstantStrategyManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler*const ch, std::vector<Solution*>const& population)
	: StrategyAdaptationManager(config, ch, population){
	assert(config.mutation.size() == 1 && config.crossover.size() == 1);
	std::fill(previousStrategies.begin(), previousStrategies.end(), 0);
}

void ConstantStrategyManager::next(std::vector<Solution*>const& population, std::map<MutationManager*, 
		std::vector<int>>& mutation, std::map<CrossoverManager*, std::vector<int>>& crossover, 
		ArrayXd& Fs, ArrayXd& Crs){

	// previousStrategies is not updated because it is constant 0
	previousFitness = ArrayXd::NullaryExpr(popSize, [population](Eigen::Index const i){
		return population[i]->getFitness();
	});

	assign(mutation, crossover, previousStrategies);

	parameterAdaptationManager->nextParameters(Fs, Crs, 
			ArrayXi::Map(previousStrategies.data(), previousStrategies.size()));
}

void ConstantStrategyManager::update(std::vector<Solution*>const& trials){
	ArrayXd const credit = ArrayXd::NullaryExpr(popSize, [trials, this](Eigen::Index const i){
		return previousFitness[i] - trials[i]->getFitness();
	}).max(0);

	parameterAdaptationManager->update(credit);
}

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

StrategyAdaptationManager::StrategyAdaptationManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler * const ch, std::vector<Solution*>const& population)
	:config(config), popSize(population.size()), K(config.crossover.size() * config.mutation.size()), 
	D(population[0]->D), parameterAdaptationManager(ParameterAdaptationManager::create(config.param)(popSize,K)), 
	activations(ArrayXi::Zero(K)){

	for (std::string m : config.mutation)
		mutationManagers.push_back(MutationManager::create(m)(ch));
	for (std::string c : config.crossover)
		crossoverManagers.push_back(CrossoverManager::create(c)());
	for (auto m : mutationManagers)
		for (auto c : crossoverManagers)
			configurations.push_back({m,c});
}

void StrategyAdaptationManager::update(std::vector<Solution*>const& population){
	ArrayXd trialF(popSize);
	for (int i = 0; i < popSize; i++)
		trialF(i) = population[i]->getFitness();
	parameterAdaptationManager->update(trialF);
}

std::vector<MutationManager*> StrategyAdaptationManager::getMutationManagers() const{
	return mutationManagers;
}

std::vector<CrossoverManager*> StrategyAdaptationManager::getCrossoverManagers() const{
	return crossoverManagers;
}

void StrategyAdaptationManager::updateActivations(std::vector<int> const& assignment){
	for (int i : assignment)
		activations(i)++;
}

ArrayXi StrategyAdaptationManager::getActivations() const{
	return activations;
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
	creditManager(CreditManager::create(config.diversity)()),
	rewardManager(RewardManager::create(config.reward)(K)),
	qualityManager(QualityManager::create(config.quality)(K)),
	probabilityManager(ProbabilityManager::create(config.probability)(K)), 
	p(ArrayXd::Constant(K, 1./K)), q(ArrayXd::Zero(K)){
}

AdaptiveStrategyManager::~AdaptiveStrategyManager(){
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
		rouletteSelect(range(K), std::vector<double>(p.data(), p.data() + p.size()), popSize, true); 
	previousFitness = 
		ArrayXd::NullaryExpr(popSize, [population](Eigen::Index const i){return population[i]->getFitness();});
	updateActivations(previousStrategies);

	mutation.clear(); 
	crossover.clear();
	for (int i = 0; i < popSize; i++){
		MutationManager* const m = std::get<0>(configurations[previousStrategies[i]]);
		CrossoverManager* const c = std::get<1>(configurations[previousStrategies[i]]);
		if (!mutation.count(m)) mutation[m] = {};
		if (!crossover.count(c)) crossover[c] = {};
		mutation[m].push_back(i); 
		crossover[c].push_back(i);
	}

	parameterAdaptationManager->nextParameters(Fs, Crs, ArrayXi::Map(previousStrategies.data(), previousStrategies.size()));
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

	qualityManager->updateQuality(q, r, p);
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

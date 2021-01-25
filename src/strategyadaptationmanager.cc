#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <exception>
#include "strategyadaptationmanager.h"
#include "rewardmanager.h"
#include "util.h"
#include "qualitymanager.h"

StrategyAdaptationManager::StrategyAdaptationManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler * const ch, std::vector<Solution*>const& population)
	:config(config), popSize(population.size()), K(config.crossover.size() * config.mutation.size()), 
	D(population[0]->D), parameterAdaptationManager(ParameterAdaptationManager::create(config.param)(population,K)){

	for (std::string m : config.mutation)
		mutationManagers.push_back(MutationManager::create(m)(ch));
	for (std::string c : config.crossover)
		crossoverManagers.push_back(CrossoverManager::create(c)());
	for (auto m : mutationManagers)
		for (auto c : crossoverManagers)
			configurations.push_back({m,c});
}

void StrategyAdaptationManager::update(std::vector<Solution*>const& population){
	Eigen::VectorXd trialF(popSize);
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

void ConstantStrategyManager::next(std::vector<Solution*> const& /*population*/, std::map<MutationManager*, 
		Eigen::VectorXi>& mutation, std::map<CrossoverManager*, Eigen::VectorXi>& crossover, Eigen::VectorXd& Fs, 
		Eigen::VectorXd& Crs){

	Eigen::VectorXi key(popSize);
	std::iota(key.begin(), key.end(), 0); // All indices [0,M-1]
	mutation[std::get<0>(configurations.front())] = key;
	crossover[std::get<1>(configurations.front())] = key;

	Eigen::VectorXi assignment(popSize, 0);
	parameterAdaptationManager->nextParameters(Fs, Crs, assignment);
}

AdaptiveStrategyManager::AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler*const ch, std::vector<Solution*>const& population)
	: StrategyAdaptationManager(config, ch, population), 
	rewardManager(RewardManager::create(config.reward)(K)),
	qualityManager(QualityManager::create(config.quality)(K)),
	probabilityManager(ProbabilityManager::create(config.probability)(K)), 
	alpha(.8), p(K, 1./K), q(K, 0.), previousStrategies(popSize), previousFitness(popSize), used(K){
}

AdaptiveStrategyManager::~AdaptiveStrategyManager(){
	delete rewardManager;
	delete probabilityManager;
}

void AdaptiveStrategyManager::next(std::vector<Solution*>const& population, std::map<MutationManager*, 
		Eigen::VectorXi>& mutation, std::map<CrossoverManager*, Eigen::VectorXi>& crossover, 
		Eigen::VectorXd& Fs, Eigen::VectorXd& Crs){
	Eigen::VectorXi const assignment = rouletteSelect(range(K), p, popSize, true);

	// Update which strategies are used in this iteration
	std::fill(used.begin(), used.end(), false);
	for (int a : assignment) used[a] = true;

	previousStrategies = assignment;
	previousMean = getMean(population);
	previousDistances = getDistances(population, previousMean);

	mutation.clear(); crossover.clear();
	for (int i = 0; i < popSize; i++){
		previousFitness[i] = population[i]->getFitness();
		auto const [m, c] = configurations[assignment[i]];
		if (!mutation.count(m)) mutation[m] = {};
		if (!crossover.count(c)) crossover[c] = {};
		mutation[m].push_back(i); 
		crossover[c].push_back(i);
	}

	parameterAdaptationManager->nextParameters(Fs, Crs, assignment);
}

void AdaptiveStrategyManager::update(std::vector<Solution*>const& trials){
	Eigen::VectorXd improvement(popSize);

	for (int i = 0; i < popSize; i++){
		double const f = trials[i]->getFitness();
		improvement[i] = (f < previousFitness[i] ? previousFitness[i] - f : 0.);
	}

	Eigen::VectorXd const currentDistances = getDistances(trials, previousMean); // distance to mean for each individual
	Eigen::VectorXd diversityFactor = (currentDistances - previousDistances) / previousDistances.mean();
	
	// Don't punish exploitation, only reward exploration
	std::transform(diversityFactor.begin(), diversityFactor.end(), diversityFactor.begin(), 
			[](double const& x){return (x > 0. ? pow(1.+x,2.) : 1.);}); 

	std::transform(improvement.begin(), improvement.end(), diversityFactor.begin(), 
			improvement.begin(), [](double const& x, double const& y){return x*y;}); 

	Eigen::VectorXd const r = rewardManager->getReward(improvement, previousStrategies);

	qualityManager->updateQuality(q, r, p);
	probabilityManager->updateProbability(p, q);
	parameterAdaptationManager->update(improvement);
}

Eigen::VectorXd AdaptiveStrategyManager::getMean(std::vector<Solution*>const& population) const{
	Eigen::VectorXd mean = Eigen::VectorXd::Zero(D); // Mean position of the entire population
	for (Solution* s : population) 
		mean += s->getX();
	mean /= popSize;

	return mean;
}

Eigen::VectorXd AdaptiveStrategyManager::getDistances(std::vector<Solution*>const& population, 
		Eigen::VectorXd const& mean) const{

	Eigen::VectorXd distances(popSize);
	for (unsigned int i = 0; i < population.size(); i++) 
		distances[i] = distance(population[i]->getX(), mean);
	return distances; 
}

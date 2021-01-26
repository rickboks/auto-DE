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
	D(population[0]->D), parameterAdaptationManager(ParameterAdaptationManager::create(config.param)(popSize,K)){

	for (std::string m : config.mutation)
		mutationManagers.push_back(MutationManager::create(m)(ch));
	for (std::string c : config.crossover)
		crossoverManagers.push_back(CrossoverManager::create(c)());
	for (auto m : mutationManagers)
		for (auto c : crossoverManagers)
			configurations.push_back({m,c});
}

void StrategyAdaptationManager::update(std::vector<Solution*>const& population){
	VectorXd trialF(popSize);
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
		std::vector<int>>& mutation, std::map<CrossoverManager*, std::vector<int>>& crossover, VectorXd& Fs, 
		VectorXd& Crs){
	std::vector<int> const key = range(popSize);
	mutation[std::get<0>(configurations.front())] = key;
	crossover[std::get<1>(configurations.front())] = key;
	parameterAdaptationManager->nextParameters(Fs, Crs, VectorXi::Zero(popSize));
}

AdaptiveStrategyManager::AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, 
		ConstraintHandler*const ch, std::vector<Solution*>const& population)
	: StrategyAdaptationManager(config, ch, population), 
	rewardManager(RewardManager::create(config.reward)(K)),
	qualityManager(QualityManager::create(config.quality)(K)),
	probabilityManager(ProbabilityManager::create(config.probability)(K)), 
	p(VectorXd::Constant(K, 1./K)), q(VectorXd::Zero(K)), previousStrategies(popSize), previousFitness(popSize){
}

AdaptiveStrategyManager::~AdaptiveStrategyManager(){
	delete rewardManager;
	delete qualityManager;
	delete probabilityManager;
}

void AdaptiveStrategyManager::next(std::vector<Solution*>const& population, std::map<MutationManager*, 
		std::vector<int>>& mutation, std::map<CrossoverManager*, std::vector<int>>& crossover, 
		VectorXd& Fs, VectorXd& Crs){
	previousMean = getMean(population);
	previousDistances = getDistances(population, previousMean);

	std::vector<int> const assignment = rouletteSelect(range(K), std::vector<double>(p.data(), p.data() + p.size()), 
			popSize, true);
	previousStrategies = assignment; 

	mutation.clear(); 
	crossover.clear();
	for (int i = 0; i < popSize; i++){
		previousFitness(i) = population[i]->getFitness();
		auto const [m, c] = configurations[assignment[i]];
		if (!mutation.count(m)) mutation[m] = {};
		if (!crossover.count(c)) crossover[c] = {};
		mutation[m].push_back(i); 
		crossover[c].push_back(i);
	}

	parameterAdaptationManager->nextParameters(Fs, Crs, VectorXi::Map(assignment.data(), assignment.size()));
}

void AdaptiveStrategyManager::update(std::vector<Solution*>const& trials){
	VectorXd improvement = VectorXd::NullaryExpr(popSize, [trials, this](Eigen::Index i){
		return (trials[i]->getFitness() < previousFitness[i] ? previousFitness[i] - trials[i]->getFitness() : 0.);
	});

	VectorXd const diversityFactor = ((getDistances(trials, previousMean) - previousDistances) / 
			previousDistances.mean()).unaryExpr( [](double const& x){return (x > 0. ? pow(1.+x,2.) : 1.);});

	improvement = improvement.array() * diversityFactor.array();

	VectorXd const r = rewardManager->getReward(improvement, 
			VectorXi::Map(previousStrategies.data(), previousStrategies.size()));

	qualityManager->updateQuality(q, r, p);
	probabilityManager->updateProbability(p, q);
	parameterAdaptationManager->update(improvement);
}

VectorXd AdaptiveStrategyManager::getMean(std::vector<Solution*>const& population) const{
	VectorXd mean = VectorXd::Zero(D); // Mean position of the entire population
	for (Solution* s : population) 
		mean += s->getX();
	mean /= popSize;
	return mean;
}

VectorXd AdaptiveStrategyManager::getDistances(std::vector<Solution*>const& population, // Distances w.r.t. mean
		VectorXd const& mean) const{
	return VectorXd::NullaryExpr(popSize, [population, mean](Eigen::Index i){
			return distance(population[i]->getX(), mean);
		}); 
}

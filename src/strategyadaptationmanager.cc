#include <algorithm>
#include <numeric>
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
	: StrategyAdaptationManager(configSpace, popSize), alpha(.4), beta(.6), pMin(.2/configs.size()), 
	pMax(pMin + 1. - configs.size() * pMin), p(configs.size(), 1./configs.size()), q(configs.size(), 0.), 
	previousStrategies(popSize), indices(range(popSize)){
}

void AdapSSManager::nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
		std::map<CrossoverManager*, std::vector<int>>& crossover){
	mutation.clear(); 
	crossover.clear();

	printVec(p);
	std::vector<int> assignment = rouletteSelect<int>(indices, p, popSize, true);
	previousStrategies = assignment;

	for (int i = 0; i < popSize; i++){
		auto [m, c] = configs[assignment[i]];
		if (!mutation.count(m)) mutation[m] = {};
		if (!crossover.count(c)) crossover[c] = {};
		mutation[m].push_back(i); 
		crossover[c].push_back(i);
	}
}

void AdapSSManager::update(std::vector<double>const& parentF, std::vector<double>const& trialF){
	std::vector<std::vector<double>> deltas(configs.size(), std::vector<double>());
	double const bestF = *std::min_element(trialF.begin(), trialF.end());

	for (int i = 0; i < popSize; i++){
		double const delta = (trialF <= parentF ? (bestF / trialF[i]) * std::abs(parentF[i] - trialF[i]) : 0.);
		deltas[previousStrategies[i]].push_back(delta);
	}

	std::vector<double> const r = reward(deltas);
	updateQuality(r);
	updateProbability();
}

// Average Normalized Reward
std::vector<double> AdapSSManager::reward(std::vector<std::vector<double>>const& deltas) const{
	std::vector<double> r(configs.size());

	for (unsigned int i = 0; i < configs.size(); i++){
		if (!deltas.empty())
			r[i] = std::accumulate(deltas[i].begin(), deltas[i].end(), 0.) / deltas[i].size();
		else 
			r[i] = 0.;
	}

	double const maxR = *std::max_element(r.begin(), r.end());
	if (maxR > 0.)
		std::transform(r.begin(), r.end(), r.begin(), [maxR](double const& x){return x/maxR;});

	return r;
}

void AdapSSManager::updateQuality(std::vector<double>const& r){
	for (unsigned int i = 0; i < configs.size(); i++)
		q[i] += alpha * (r[i] - q[i]);
}

// Adaptive Pursuit
void AdapSSManager::updateProbability(){
	unsigned int const bestIdx = std::distance(q.begin(), std::max_element(q.begin(), q.end()));
	for (unsigned int i = 0; i < configs.size(); i++){
		if (i == bestIdx)
			p[i] += beta * (pMax - p[i]);
		else 
			p[i] += beta * (pMin - p[i]);
	}
}

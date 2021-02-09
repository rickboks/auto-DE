#pragma once
#include <string>
#include <vector>
#include <map>
#include "Eigen/Dense"

using Eigen::ArrayXd;
using Eigen::ArrayXi;

struct StrategyAdaptationConfiguration {
	StrategyAdaptationConfiguration(std::vector<std::string> const mutation, std::vector<std::string> const crossover,
									std::string const param, std::string diversity, std::string const reward, 
									std::string const quality, std::string const probability)
	: mutation(mutation), crossover(crossover), param(param), diversity(diversity), reward(reward), quality(quality), 
	probability(probability){};
	std::vector<std::string> const mutation, crossover;
	std::string const param, diversity, reward, quality, probability;
};

class RewardManager; 
class QualityManager;
class CreditManager;
class ProbabilityManager;
class ParameterAdaptationManager;
class MutationManager;
class CrossoverManager;
class ConstraintHandler;
class Solution;

class StrategyAdaptationManager {
	public:
		StrategyAdaptationManager(StrategyAdaptationConfiguration const config, ConstraintHandler *const ch, 
				std::vector<Solution*>const& population);
		virtual ~StrategyAdaptationManager();
		virtual void next(std::vector<Solution*>const& population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, ArrayXd& Fs, ArrayXd& Crs)=0;
		virtual void update(std::vector<Solution*>const& trials);
		std::vector<MutationManager*> getMutationManagers() const;
		std::vector<CrossoverManager*> getCrossoverManagers() const;
		ArrayXi getActivations() const;
	protected:
		StrategyAdaptationConfiguration const config;
		std::vector<MutationManager*> mutationManagers; 
		std::vector<CrossoverManager*> crossoverManagers; 
		std::vector<std::tuple<MutationManager*, CrossoverManager*>> configurations;
		int const popSize;
		int const K;
		int const D;
		ParameterAdaptationManager* const parameterAdaptationManager;
		ArrayXi activations;
		void updateActivations(std::vector<int> const& assignment);
};

class ConstantStrategyManager : public StrategyAdaptationManager {
	public:
		ConstantStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, 
				std::vector<Solution*>const& population);
		void next(std::vector<Solution*>const& population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, 
				ArrayXd& Fs, ArrayXd& Crs);
};

class AdaptiveStrategyManager : public StrategyAdaptationManager {
	private:
		CreditManager const* const creditManager;
		RewardManager const* const rewardManager;
		QualityManager const* const qualityManager;
		ProbabilityManager const* const probabilityManager;
		double const alpha = .3;
		ArrayXd p; 
		ArrayXd q; 
		std::vector<int> previousStrategies;
		ArrayXd previousFitness; 
		ArrayXd previousDistances; // Distances of all K configs
		ArrayXd previousMean;
		ArrayXd getMean(std::vector<Solution*>const& population) const;
		ArrayXd getDistances(std::vector<Solution*>const& population, ArrayXd const& mean) const;
	public:
		AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, 
				std::vector<Solution*>const& population);
		~AdaptiveStrategyManager();
		void next(std::vector<Solution*>const & population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, 
				ArrayXd& Fs, ArrayXd& Crs);
		void update(std::vector<Solution*>const& trials);
};

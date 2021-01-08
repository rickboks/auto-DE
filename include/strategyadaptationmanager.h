#pragma once
#include <vector>
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "probabilitymanager.h"

struct StrategyAdaptationConfiguration {
	StrategyAdaptationConfiguration(std::vector<std::string> const mutation, std::vector<std::string> const crossover,
									std::string const reward, std::string const probability)
	: mutation(mutation), crossover(crossover), reward(reward), probability(probability){};

	std::vector<std::string> const mutation, crossover;
	std::string const reward, probability;
};

class RewardManager;

class StrategyAdaptationManager {
	public:
		StrategyAdaptationManager(StrategyAdaptationConfiguration const config, ConstraintHandler *const ch, int const popSize);
		virtual ~StrategyAdaptationManager();
		virtual void nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover)=0;
		virtual void update(std::vector<double>const& /*parentF*/, std::vector<double>const& /*trialF*/){};
		std::vector<MutationManager*> getMutationManagers() const;
		std::vector<CrossoverManager*> getCrossoverManagers() const;
	protected:
		StrategyAdaptationConfiguration const config;
		std::vector<MutationManager*> mutationManagers; 
		std::vector<CrossoverManager*> crossoverManagers; 
		std::vector<std::tuple<MutationManager*, CrossoverManager*>> configurations;
		int const popSize;
		int const K;
};

class ConstantStrategyManager : public StrategyAdaptationManager {
	public:
		ConstantStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, int const popSize);
		void nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover);
};

class AdaptiveStrategyManager : public StrategyAdaptationManager {
	private:
		RewardManager const* const rewardManager;
		ProbabilityManager const* const probabilityManager;
		double const alpha;
		std::vector<double> p; 
		std::vector<double> q; 
		std::vector<int> previousStrategies;
		std::vector<int> indices; // simply a list from 0 .. M-1
		void updateQuality(std::vector<double> const r);
	public:
		AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, int const popSize);
		~AdaptiveStrategyManager();
		void nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover);
		void update(std::vector<double>const& parentF, std::vector<double>const& trialF);
};

#pragma once
#include <vector>
#include "mutationmanager.h"
#include "crossovermanager.h"

struct StrategyAdaptationConfiguration {
	StrategyAdaptationConfiguration(std::vector<std::string> const mutation, std::vector<std::string> const crossover,
									std::string const reward)
	: mutation(mutation), crossover(crossover), reward(reward){};

	std::vector<std::string> const mutation, crossover;
	std::string const reward;
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
		double const alpha;
		double const beta;
		double const pMin;
		double const pMax;
		std::vector<double> p; 
		std::vector<double> q; 
		std::vector<int> previousStrategies;
		std::vector<int> indices; // simply a list from 0 .. M-1
		void updateQuality(std::vector<double> const& r);
		void updateProbability();
	public:
		AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, int const popSize);
		~AdaptiveStrategyManager();
		void nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover);
		void update(std::vector<double>const& parentF, std::vector<double>const& trialF);
};

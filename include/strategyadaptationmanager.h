#pragma once
#include <vector>
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "parameteradaptationmanager.h"
#include "probabilitymanager.h"

struct StrategyAdaptationConfiguration {
	StrategyAdaptationConfiguration(std::vector<std::string> const mutation, std::vector<std::string> const crossover,
									std::string const param, std::string const fitnessReward, std::string const probability)
	: mutation(mutation), crossover(crossover), param(param), fitnessReward(fitnessReward), probability(probability){};
	std::vector<std::string> const mutation, crossover;
	std::string const param, fitnessReward, probability;
};

class FitnessRewardManager;

class StrategyAdaptationManager {
	public:
		StrategyAdaptationManager(StrategyAdaptationConfiguration const config, ConstraintHandler *const ch, 
				std::vector<Solution*>const& population);
		virtual ~StrategyAdaptationManager();
		virtual void next(std::vector<Solution*>const& population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, 
				std::vector<double>& Fs, std::vector<double>& Crs)=0;
		virtual void update(std::vector<Solution*>const& population);
		std::vector<MutationManager*> getMutationManagers() const;
		std::vector<CrossoverManager*> getCrossoverManagers() const;
	protected:
		StrategyAdaptationConfiguration const config;
		std::vector<MutationManager*> mutationManagers; 
		std::vector<CrossoverManager*> crossoverManagers; 
		std::vector<std::tuple<MutationManager*, CrossoverManager*>> configurations;
		int const popSize;
		int const K;
		int const D;
		ParameterAdaptationManager* parameterAdaptationManager;
};

class ConstantStrategyManager : public StrategyAdaptationManager {
	public:
		ConstantStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, 
				std::vector<Solution*>const& population);
		void next(std::vector<Solution*>const& population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, 
				std::vector<double>& Fs, std::vector<double>& Crs);
};

class AdaptiveStrategyManager : public StrategyAdaptationManager {
	private:
		FitnessRewardManager const* const fitnessRewardManager;
		ProbabilityManager const* const probabilityManager;
		double const alpha;
		std::vector<double> p; 
		std::vector<double> q; 
		std::vector<int> previousStrategies;
		std::vector<double> previousFitness; 
		std::vector<std::vector<double>> diversityBefore; // Diversity of all K configs on D dimensions

		std::vector<std::vector<double>> getDiversity(std::vector<Solution*>const& population, 
				std::vector<int> const& assignment) const;
		void updateQuality(std::vector<double> const& r);
	public:
		AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, 
				std::vector<Solution*>const& population);
		~AdaptiveStrategyManager();
		void next(std::vector<Solution*>const& population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, 
				std::vector<double>& Fs, std::vector<double>& Crs);
		void update(std::vector<Solution*>const& population);
};

#pragma once
#include <string>
#include <vector>
#include <map>
#include "Eigen/Dense"

using Eigen::ArrayXd;
using Eigen::ArrayXi;

struct StrategyAdaptationConfiguration {
	std::vector<std::string> const mutation, crossover;
	std::string const param, credit, reward, quality, probability;
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
		static std::function<StrategyAdaptationManager* (StrategyAdaptationConfiguration const, ConstraintHandler *const, 
				std::vector<Solution*>const&)> create(std::string const id);

		StrategyAdaptationManager(StrategyAdaptationConfiguration const config, ConstraintHandler *const ch, 
				std::vector<Solution*>const& population);
		virtual ~StrategyAdaptationManager();
		virtual void next(std::vector<Solution*>const& population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, ArrayXd& Fs, ArrayXd& Crs)=0;
		virtual void update(std::vector<Solution*>const& trials)=0;
		std::vector<MutationManager*> getMutationManagers() const;
		std::vector<CrossoverManager*> getCrossoverManagers() const;
		ArrayXi getLastActivations() const;
		ArrayXd getDistancesToMeanPosition() const;
		std::vector<std::string> getConfigurationIDs() const;
		int const K;
	protected:
		StrategyAdaptationConfiguration const config;
		std::vector<MutationManager*> mutationManagers; 
		std::vector<CrossoverManager*> crossoverManagers; 
		std::vector<std::tuple<MutationManager*, CrossoverManager*>> configurations;
		std::vector<std::string> configurationIDs;
		int const popSize;
		int const D;
		ParameterAdaptationManager* const parameterAdaptationManager;
		ArrayXd previousFitness; 
		ArrayXd previousMean;
		ArrayXd previousDistances; // Distances of all K configs
		std::vector<int> previousStrategies;
		void assign(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, std::vector<int> const& assignment);
		ArrayXd getMean(std::vector<Solution*>const& population) const;
		ArrayXd getDistances(std::vector<Solution*>const& population, ArrayXd const& mean) const;
};

class AdaptiveStrategyManager : public StrategyAdaptationManager {
	private:
		CreditManager const* const creditManager;
		RewardManager const* const rewardManager;
		QualityManager const* const qualityManager;
		ProbabilityManager const* const probabilityManager;
		ArrayXd p; 
		ArrayXd q; 
		ArrayXd used;	
	public:
		AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, 
				std::vector<Solution*>const& population);
		~AdaptiveStrategyManager();
		void next(std::vector<Solution*>const & population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, 
				ArrayXd& Fs, ArrayXd& Crs);
		void update(std::vector<Solution*>const& trials);
};

class RandomStrategyManager : public StrategyAdaptationManager {
	public:
		RandomStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, 
				std::vector<Solution*>const& population);
		void next(std::vector<Solution*>const & population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, ArrayXd& Fs, ArrayXd& Crs);
		void update(std::vector<Solution*>const& trials);
};

class ConstantStrategyManager : public StrategyAdaptationManager {
	private:
		CreditManager const* const creditManager;
	public:
		ConstantStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, 
				std::vector<Solution*>const& population);
		~ConstantStrategyManager();
		void next(std::vector<Solution*>const & population, std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover, ArrayXd& Fs, ArrayXd& Crs);
		void update(std::vector<Solution*>const& trials);
};

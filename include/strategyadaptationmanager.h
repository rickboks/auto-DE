#pragma once
#include <vector>
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "parameteradaptationmanager.h"
#include "probabilitymanager.h"

struct StrategyAdaptationConfiguration {
	StrategyAdaptationConfiguration(std::vector<std::string> const mutation, std::vector<std::string> const crossover,
									std::string const param, std::string const reward, std::string const quality,
									std::string const probability)
	: mutation(mutation), crossover(crossover), param(param), reward(reward), quality(quality), probability(probability){};
	std::vector<std::string> const mutation, crossover;
	std::string const param, reward, quality, probability;
};

class RewardManager; 
class QualityManager;

class StrategyAdaptationManager {
	public:
		StrategyAdaptationManager(StrategyAdaptationConfiguration const config, ConstraintHandler *const ch, 
				std::vector<Solution*>const& population);
		virtual ~StrategyAdaptationManager();
		virtual void next(std::vector<Solution*>const& population, std::map<MutationManager*, Eigen::VectorXi>& mutation, 
				std::map<CrossoverManager*, Eigen::VectorXi>& crossover, 
				Eigen::VectorXd& Fs, Eigen::VectorXd& Crs)=0;
		virtual void update(std::vector<Solution*>const& trials);
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
		void next(std::vector<Solution*>const& population, std::map<MutationManager*, Eigen::VectorXi>& mutation, 
				std::map<CrossoverManager*, Eigen::VectorXi>& crossover, 
				Eigen::VectorXd& Fs, Eigen::VectorXd& Crs);
};

class AdaptiveStrategyManager : public StrategyAdaptationManager {
	private:
		RewardManager const* const rewardManager;
		QualityManager const* const qualityManager;
		ProbabilityManager const* const probabilityManager;
		double const alpha;
		Eigen::VectorXd p; 
		Eigen::VectorXd q; 
		Eigen::VectorXd previousStrategies;
		Eigen::VectorXd previousFitness; 
		Eigen::VectorXd previousDistances; // Distances of all K configs
		Eigen::VectorXd previousMean;
		std::vector<bool> used;
		Eigen::VectorXd getMean(std::vector<Solution*>const& population) const;
		Eigen::VectorXd getDistances(std::vector<Solution*>const& population, Eigen::VectorXd const& mean) const;
	public:
		AdaptiveStrategyManager(StrategyAdaptationConfiguration const config, ConstraintHandler* const ch, 
				std::vector<Solution*>const& population);
		~AdaptiveStrategyManager();
		void next(std::vector<Solution*>const & population, std::map<MutationManager*, Eigen::VectorXi>& mutation, 
				std::map<CrossoverManager*, Eigen::VectorXi>& crossover, 
				Eigen::VectorXd& Fs, Eigen::VectorXd& Crs);
		void update(std::vector<Solution*>const& trials);
};

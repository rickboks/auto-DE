#pragma once
#include <vector>
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "configurationspace.h"

class StrategyAdaptationManager {
	public:
		StrategyAdaptationManager(ConfigurationSpace const* const configSpace, int const popSize);
		virtual ~StrategyAdaptationManager(){};
		virtual void nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover)=0;
		virtual void update(std::vector<double>const /*parentF*/, std::vector<double>const /*trialF*/){};
	protected:
		ConfigurationSpace const* const configSpace;
		std::vector<std::tuple<MutationManager*, CrossoverManager*>> configs;
		int const popSize;
};

class ConstantStrategyManager : public StrategyAdaptationManager {
	public:
		ConstantStrategyManager(ConfigurationSpace const* const configSpace, int const popSize);
		void nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover);
};

class AdapSSManager : public StrategyAdaptationManager {
	private:
		double const alpha;
		double const beta;
		double const pMin;
		std::vector<double> p; 
		std::vector<double> q; 
		std::vector<double> r; 
		std::vector<int> previousStrategies;
		std::vector<int> indices; // simply a list from 0 .. M-1
	public:
		AdapSSManager(ConfigurationSpace const* const configSpace, int const popSize);
		void nextStrategies(std::map<MutationManager*, std::vector<int>>& mutation, 
				std::map<CrossoverManager*, std::vector<int>>& crossover);
		void update(std::vector<double>const parentF, std::vector<double>const trialF);
};

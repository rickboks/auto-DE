#include <vector>
#include "mutationmanager.h"
#include "crossovermanager.h"

class StrategyAdaptationManager {
	public:
		StrategyAdaptationManager();
		virtual ~StrategyAdaptationManager();
		virtual void nextStrategies(std::vector<MutationManager*>& mutation, std::vector<CrossoverManager*>& crossover)=0;
	private:
};

class ConstantStrategyManager {
	public:
		ConstantStrategyManager();
		void nextStrategies(std::vector<MutationManager*>& mutation, std::vector<CrossoverManager*>& crossover);
	private:
};

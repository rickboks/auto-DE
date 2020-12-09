#include <vector>
#include "mutationmanager.h"
#include "crossovermanager.h"

class StrategyAdaptationManager {
	public:
		StrategyAdaptationManager ();
		virtual ~StrategyAdaptationManager ();
		void nextStrategies(std::vector<MutationManager*> mutation, std::vector<CrossoverManager*> crossover);
	private:
		/* data */
};

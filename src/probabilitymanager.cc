#include "probabilitymanager.h"
#include <algorithm>

#define LC(X) [](int const K){return new X(K);}
std::map<std::string, std::function<ProbabilityManager*(int const)>> const probabilityManagers ({
	{"AP", LC(AdaptivePursuitManager)}	
});

void AdaptivePursuitManager::updateProbability(std::vector<double>const& q, std::vector<double>& p) const{
	int const bestIdx = std::distance(q.begin(), std::max_element(q.begin(), q.end()));
	for (int i = 0; i < K; i++){
		if (i == bestIdx)
			p[i] += beta * (pMax - p[i]);
		else 
			p[i] += beta * (pMin - p[i]);
	}
}

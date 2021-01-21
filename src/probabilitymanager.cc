#include "probabilitymanager.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <ostream>

#define LC(X) [](int const K){return new X(K);}
std::map<std::string, std::function<ProbabilityManager*(int const)>> const probabilityManagers ({
	{"AP", LC(AdaptivePursuitManager)},
	{"PM", LC(ProbabilityMatchingManager)},
});

void AdaptivePursuitManager::updateProbability(std::vector<double>const& q, std::vector<double>& p) const {
	int const bestIdx = std::distance(q.begin(), std::max_element(q.begin(), q.end()));
	for (int i = 0; i < K; i++){
		if (i == bestIdx)
			p[i] += beta * (pMax - p[i]);
		else 
			p[i] += beta * (pMin - p[i]);
	}
}

void ProbabilityMatchingManager::updateProbability(std::vector<double>const& q, std::vector<double>& p) const {
	double const total = std::accumulate(q.begin(), q.end(), 0.);
	for (int i = 0; i < K; i++){
		p[i] = pMin + (1. - K * pMin) * (q[i] / total);
	}
}

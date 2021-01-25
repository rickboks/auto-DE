#include "probabilitymanager.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <ostream>

std::function<ProbabilityManager* (int const)> ProbabilityManager::create(std::string const id){
#define LC(X) [](int const K){return new X(K);}
	if (id == "AP") return LC(AdaptivePursuitManager);
	if (id == "PM") return LC(ProbabilityMatchingManager);
	throw std::invalid_argument("no such ProbabilityManager: " + id);
}
void AdaptivePursuitManager::updateProbability(std::vector<double>& p, std::vector<double>const& q) const {
	int const bestIdx = std::distance(q.begin(), std::max_element(q.begin(), q.end()));
	for (int i = 0; i < K; i++){
		if (i == bestIdx)
			p[i] += beta * (pMax - p[i]);
		else 
			p[i] += beta * (pMin - p[i]);
	}
}

void ProbabilityMatchingManager::updateProbability(std::vector<double>& p, std::vector<double>const& q) const {
	double const total = std::accumulate(q.begin(), q.end(), 0.);
	for (int i = 0; i < K; i++){
		p[i] = pMin + (1. - K * pMin) * (q[i] / total);
	}
}

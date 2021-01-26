#include "probabilitymanager.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <ostream>

std::function<ProbabilityManager* (int const)> ProbabilityManager::create(std::string const id){
#define ALIAS(X,Y) if (id==X) return [](int const K){return new Y(K);};
	ALIAS("AP", AdaptivePursuitManager)
	ALIAS("PM", ProbabilityMatchingManager)
	throw std::invalid_argument("no such ProbabilityManager: " + id);
}
void AdaptivePursuitManager::updateProbability(VectorXd& p, VectorXd const& q) const {
	int const bestIdx = std::distance(q.begin(), std::max_element(q.begin(), q.end()));
	for (int i = 0; i < K; i++){
		if (i == bestIdx)
			p[i] += beta * (pMax - p[i]);
		else 
			p[i] += beta * (pMin - p[i]);
	}
}

void ProbabilityMatchingManager::updateProbability(VectorXd& p, VectorXd const& q) const {
	p = pMin + ((1. - K * pMin) * (q / q.sum())).array();
}

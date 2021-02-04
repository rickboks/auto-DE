#include "probabilitymanager.h"
#include "util.h"
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

void AdaptivePursuitManager::updateProbability(ArrayXd& p, ArrayXd const& q) const {
	Eigen::Index bestIdx; q.maxCoeff(&bestIdx);
	ArrayXd const new_p = 
		ArrayXd::NullaryExpr(K, [this, bestIdx](Eigen::Index const i){return i == bestIdx ? pMax : pMin;});
	p += beta * (new_p - p); // small beta -> slow change
}

void ProbabilityMatchingManager::updateProbability(ArrayXd& p, ArrayXd const& q) const {
	p = pMin + (1. - K * pMin) * (q / q.sum());
}

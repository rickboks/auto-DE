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
void AdaptivePursuitManager::updateProbability(VectorXd& p, VectorXd const& q) const {
	Eigen::Index bestIdx; q.maxCoeff(&bestIdx);
	VectorXd const new_p = 
		VectorXd::NullaryExpr(K, [this, bestIdx](Eigen::Index const i){return i == bestIdx ? pMax : pMin;});
	p += beta * (new_p - p);
}

void ProbabilityMatchingManager::updateProbability(VectorXd& p, VectorXd const& q) const {
	p = pMin + ((1. - K * pMin) * (q / q.sum())).array();
}

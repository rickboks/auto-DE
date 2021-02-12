#include "qualitymanager.h"
#include "rng.h"
#include "Eigen/Dense"
#include <stdexcept>
#include <iostream>

using Eigen::MatrixXd;

std::function<QualityManager* (int const)> QualityManager::create(std::string const id){
#define ALIAS(X,Y) if (id==X) return [](int const K){return new Y(K);};
	ALIAS("WS", WeightedSumQuality)
	throw std::invalid_argument("no such QualityManager: " + id);
}

void WeightedSumQuality::updateQuality(ArrayXd &q, ArrayXd const& r, std::vector<bool> const used) const{
	q = ArrayXd::NullaryExpr(K, [q, r, &used, this](Eigen::Index const i){
			return used[i] ? q(i) + alpha * (r(i) - q(i)) : q(i);
		});
}

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

void WeightedSumQuality::updateQuality(ArrayXd&q, ArrayXd const& r, 
		ArrayXd const& /*p*/) const{
	q += alpha * (r - q); // small alpha -> slow change
}

#include "qualitymanager.h"
#include "Eigen/Dense"
#include <stdexcept>
#include <iostream>

using Eigen::MatrixXd;

std::function<QualityManager* (int const)> QualityManager::create(std::string const id){
#define ALIAS(X,Y) if (id==X) return [](int const K){return new Y(K);};
	ALIAS("WS", WeightedSumQuality)
	ALIAS("BM", BellmanQuality)
	throw std::invalid_argument("no such QualityManager: " + id);
}

void WeightedSumQuality::updateQuality(ArrayXd&q, ArrayXd const& r, 
		ArrayXd const& /*p*/) const{
	q = (1.-alpha) * q + alpha * r; // low alpha -> slow change
}

void BellmanQuality::updateQuality(ArrayXd&q, ArrayXd const& r, 
		ArrayXd const& p) const{
	MatrixXd P(K, K);
	P.diagonal().fill(0.);

	for (int i = 0; i < K-1; i++) // iterate over upper triangular part
		for (int j = i+1; j < K; j++)
			P(i,j) = P(j,i) = p(i) + p(j);

	q = softmax((1. - gamma * P.array()).matrix().inverse() * r.matrix());
}

ArrayXd BellmanQuality::softmax(ArrayXd const& x) const{
	ArrayXd const exp_x = x.exp();
	return exp_x / exp_x.sum();
}

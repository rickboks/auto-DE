#include "qualitymanager.h"
#include "Eigen/Dense"
#include <stdexcept>
#include <iostream>

using Eigen::MatrixXd;

std::function<QualityManager* (int const)> QualityManager::create(std::string const id){
#define LC(X) [](int const K){return new X(K);}
	if (id == "WS") return LC(WeightedSumQuality);
	if (id == "BM") return LC(BellmanQuality);
	throw std::invalid_argument("no such QualityManager: " + id);
}

void WeightedSumQuality::updateQuality(VectorXd&q, VectorXd const& r, 
		VectorXd const& /*p*/) const{
	q += alpha * (r - q);
}

void BellmanQuality::updateQuality(VectorXd&q, VectorXd const& r, 
		VectorXd const& p) const{
	MatrixXd P(K, K);
	P.diagonal().fill(0.);

	for (int i = 0; i < K-1; i++){ // iterate over upper triangular part
		for (int j = i+1; j < K; j++)
			P(i,j) = P(j,i) = p[i] + p[j];
	}
	q = softmax((1. - (gamma * P.array())).matrix().inverse() * r);
}

VectorXd BellmanQuality::softmax(VectorXd const& x) const{
	VectorXd const exp_x = x.array().exp();
	return exp_x / exp_x.sum();
}

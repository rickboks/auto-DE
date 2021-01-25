#include "qualitymanager.h"
#include "Eigen/Dense"
#include <stdexcept>
#include <iostream>

std::function<QualityManager* (int const)> QualityManager::create(std::string const id){
#define LC(X) [](int const K){return new X(K);}
	if (id == "WS") return LC(WeightedSumQuality);
	throw std::invalid_argument("no such QualityManager: " + id);
}

void WeightedSumQuality::updateQuality(Eigen::VectorXd&q, Eigen::VectorXd const& r, 
		Eigen::VectorXd const& /*p*/) const{
	q += alpha * (r - q);
}

void RecPMQuality::updateQuality(Eigen::VectorXd&q, Eigen::VectorXd const& r, 
		Eigen::VectorXd const& p) const{
	//Eigen::MatrixXd P (K, K);
	//for (int i = 0; i < K; i++)
		//for (int j = 0; j < K; j++)
			//P(i,j)= 1 - gamma*(p[i] + p[j]);

	//q = P.inverse() * Eigen::VectorXd(r);
}

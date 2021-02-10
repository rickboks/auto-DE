#include "creditmanager.h"
#include <iostream>
using Eigen::Vector2d;
using Eigen::MatrixXd;

std::function<CreditManager* ()> CreditManager::create(std::string const id){
#define ALIAS(X, Y) if (id == X) return [](){return new Y();};
	ALIAS("DR", DiversityRatio)
	ALIAS("SD", SquaredDiversityRatio)
	ALIAS("FD", FitnessScaledByDiversityRatio)
	ALIAS("FS", FitnessScaledBySquaredDiversityRatio)
	ALIAS("FI", FitnessImprovement)
	ALIAS("CO", Compass)
	throw std::invalid_argument("no such CreditManager: " + id);
}

ArrayXd DiversityRatio::getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
	ArrayXd const& currentDistances) const {
	return fitnessDeltas.unaryExpr([](double const& x){
		return x > 0. ? 1. : 0.;
	}) * (currentDistances / previousDistances);
}

ArrayXd SquaredDiversityRatio::getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
	ArrayXd const& currentDistances) const {
	return fitnessDeltas.unaryExpr([](double const& x){
		return x > 0. ? 1. : 0.;
	}) * (currentDistances / previousDistances).pow(2);
}

ArrayXd FitnessScaledByDiversityRatio::getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
	ArrayXd const& currentDistances) const {
	return fitnessDeltas.max(0) * (currentDistances / previousDistances);
}

ArrayXd FitnessScaledBySquaredDiversityRatio::getCredit(ArrayXd const& fitnessDeltas, 
		ArrayXd const& previousDistances, ArrayXd const& currentDistances) const {
	return fitnessDeltas.max(0) * (currentDistances / previousDistances).pow(2);
}

ArrayXd FitnessImprovement::getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& /*previousDistances*/, 
	ArrayXd const& /*currentDistances*/) const {
	return fitnessDeltas.max(0);
}

ArrayXd Compass::getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
	ArrayXd const& currentDistances) const {
	ArrayXd const diversityDeltas = currentDistances - previousDistances;

	MatrixXd vectors(2, fitnessDeltas.size());
	vectors.row(0) = diversityDeltas / diversityDeltas.abs().maxCoeff();
	vectors.row(1) = fitnessDeltas / fitnessDeltas.abs().maxCoeff();
	vectors.colwise().normalize();

	ArrayXd angles = ArrayXd::NullaryExpr(vectors.cols(), 
			[this,&vectors](Eigen::Index const i){
				return c.dot(vectors.col(i));
			}
	);
	angles -= angles.minCoeff();

	return fitnessDeltas;
}

#include "diversitymanager.h"
#include <iostream>

using Eigen::Vector2d, Eigen::MatrixXd;
std::function<DiversityManager* ()> DiversityManager::create(std::string const id){
#define ALIAS(X, Y) if (id == X) return [](){return new Y();};
	ALIAS("FS", FitnessScaledByDiversity)
	ALIAS("DR", DiversityRatio)
	ALIAS("FI", FitnessImprovement)
	ALIAS("CO", Compass)
	throw std::invalid_argument("no such DiversityManager: " + id);
}

ArrayXd FitnessScaledByDiversity::rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
	ArrayXd const& currentDistances) const{
	return fitnessDeltas.max(0) * (1. + (currentDistances - previousDistances) / previousDistances.mean()).max(1).pow(2);
}

ArrayXd DiversityRatio::rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
	ArrayXd const& currentDistances) const {
	return fitnessDeltas.unaryExpr([](double const& x){
		return x > 0. ? 1. : 0.;}
	) * (currentDistances / previousDistances);
}

ArrayXd FitnessImprovement::rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& /*previousDistances*/, 
	ArrayXd const& /*currentDistances*/) const {
	return fitnessDeltas.max(0);
}

ArrayXd Compass::rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
	ArrayXd const& currentDistances) const {
	ArrayXd const diversityDeltas = previousDistances - currentDistances;

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

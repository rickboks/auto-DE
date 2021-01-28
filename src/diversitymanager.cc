#include "diversitymanager.h"

std::function<DiversityManager* ()> DiversityManager::create(std::string const id){
#define ALIAS(X, Y) if (id == X) return [](){return new Y();};
	ALIAS("SF", FitnessScaledByDiversity)
	ALIAS("DF", DiversityFraction)
	ALIAS("FI", FitnessImprovement)
	throw std::invalid_argument("no such DiversityManager: " + id);
}

ArrayXd FitnessScaledByDiversity::rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
	ArrayXd const& currentDistances) const{
	return fitnessDeltas.max(0) * (1. + (currentDistances - previousDistances) / previousDistances.mean()).max(1).pow(2);
}

ArrayXd DiversityFraction::rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
	ArrayXd const& currentDistances) const {
	return fitnessDeltas.unaryExpr([](double const& x){
		return x > 0. ? 1. : 0.;}
	) * (currentDistances / previousDistances);
}

ArrayXd FitnessImprovement::rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& /*previousDistances*/, 
	ArrayXd const& /*currentDistances*/) const {
	return fitnessDeltas.max(0);
}

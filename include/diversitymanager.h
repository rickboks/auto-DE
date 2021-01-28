#pragma once
#include "Eigen/Dense"

using Eigen::ArrayXd;
class DiversityManager {
	public:
		static std::function<DiversityManager* ()> create(std::string const id);
		DiversityManager(){};
		virtual ~DiversityManager (){};
		virtual ArrayXd rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const=0;
};

class FitnessScaledByDiversity : public DiversityManager {
	public:
		ArrayXd rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class DiversityImprovement : public DiversityManager {
	public:
		ArrayXd rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

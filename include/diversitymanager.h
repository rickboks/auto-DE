#pragma once
#include "Eigen/Dense"
#include <cmath>

constexpr double pi() { return std::atan(1)*4; }

using Eigen::ArrayXd;
using Eigen::Vector2d;
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

class DiversityRatio : public DiversityManager {
	public:
		ArrayXd rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class DiversityRatioSquared : public DiversityManager {
	public:
		ArrayXd rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class DiversityRatioSquaredImprovement : public DiversityManager {
	public:
		ArrayXd rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};


class FitnessImprovement : public DiversityManager {
	public:
		ArrayXd rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class Compass : public DiversityManager {
	private:
		double const omega = pi() / 4.;
		Vector2d const c;
	public:
		Compass() : c(cos(omega), sin(omega)){};
		ArrayXd rewardDiversity(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

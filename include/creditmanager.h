#pragma once
#include "Eigen/Dense"
#include "params.h"
#include <cmath>
using Eigen::ArrayXd;
using Eigen::Vector2d;
class CreditManager {
	public:
		static std::function<CreditManager* ()> create(std::string const id);
		CreditManager(){};
		virtual ~CreditManager (){};
		virtual ArrayXd getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const=0;
};

class FitnessScaledByDiversityRatio : public CreditManager {
	public:
		ArrayXd getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class FitnessScaledBySquaredDiversityRatio : public CreditManager {
	public:
		ArrayXd getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class DiversityRatio : public CreditManager {
	public:
		ArrayXd getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class SquaredDiversityRatio: public CreditManager {
	public:
		ArrayXd getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class FitnessImprovement : public CreditManager {
	public:
		ArrayXd getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class Compass : public CreditManager {
	private:
		double const omega = params::CO_omega;
		Vector2d const c;
	public:
		Compass() : c(cos(omega), sin(omega)){};
		ArrayXd getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

class ParetoDominance : public CreditManager {
	public: 
		ArrayXd getCredit(ArrayXd const& fitnessDeltas, ArrayXd const& previousDistances, 
				ArrayXd const& currentDistances) const;
};

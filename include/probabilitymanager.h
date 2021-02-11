#pragma once
#include <functional>
#include <string>
#include <vector>
#include <map>
#include "Eigen/Dense"
#include "params.h"

using Eigen::ArrayXd;
class ProbabilityManager {
	protected:
		int const K;
		double const pMin;
	public:
		static std::function<ProbabilityManager* (int const)> create(std::string const id);
		ProbabilityManager(int const K): K(K), pMin(params::PM_AP_pMin(K)){};
		virtual ~ProbabilityManager (){};
		virtual void updateProbability(ArrayXd& p, ArrayXd const& q) const=0;
};

class AdaptivePursuitManager : public ProbabilityManager {
	private:
		double const beta = params::AP_beta;
		double const pMax;
	public:
		AdaptivePursuitManager (int const K): ProbabilityManager(K), pMax(params::AP_pMax(K)){};
		void updateProbability(ArrayXd& p, ArrayXd const& q) const;
};

class ProbabilityMatchingManager : public ProbabilityManager {
	public:
		ProbabilityMatchingManager (int const K): ProbabilityManager(K){};
		void updateProbability(ArrayXd& p, ArrayXd const& q) const;
};

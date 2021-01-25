#pragma once
#include <functional>
#include <string>
#include <vector>
#include <map>
#include "Eigen/Dense"

using Eigen::VectorXd;
class ProbabilityManager {
	protected:
		int const K;
		double const pMin;
	public:
		static std::function<ProbabilityManager* (int const)> create(std::string const id);
		ProbabilityManager(int const K): K(K), pMin(.2/K){};
		virtual ~ProbabilityManager (){};
		virtual void updateProbability(VectorXd& p, VectorXd const& q) const=0;
};

class AdaptivePursuitManager : public ProbabilityManager {
	private:
		double const beta = .8;
		double const pMax;
	public:
		AdaptivePursuitManager (int const K): ProbabilityManager(K), beta(.8), pMax(pMin + 1. - K * pMin){};
		void updateProbability(VectorXd& p, VectorXd const& q) const;
};

class ProbabilityMatchingManager : public ProbabilityManager {
	public:
		ProbabilityMatchingManager (int const K): ProbabilityManager(K){};
		void updateProbability(VectorXd& p, VectorXd const& q) const;
};

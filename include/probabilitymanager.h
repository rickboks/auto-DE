#pragma once
#include <functional>
#include <string>
#include <vector>
#include <map>
#include "Eigen/Dense"

using Eigen::ArrayXd;
class ProbabilityManager {
	protected:
		int const K;
		double const pMin;
	public:
		static std::function<ProbabilityManager* (int const)> create(std::string const id);
		ProbabilityManager(int const K): K(K), pMin(1./(4.*K)){};
		virtual ~ProbabilityManager (){};
		virtual void updateProbability(ArrayXd& p, ArrayXd const& q) const=0;
};

class AdaptivePursuitManager : public ProbabilityManager {
	private:
		double const beta = .6;
		double const pMax;
	public:
		AdaptivePursuitManager (int const K): ProbabilityManager(K), pMax(1.-(K-1.)*pMin){};
		void updateProbability(ArrayXd& p, ArrayXd const& q) const;
};

class ProbabilityMatchingManager : public ProbabilityManager {
	public:
		ProbabilityMatchingManager (int const K): ProbabilityManager(K){};
		void updateProbability(ArrayXd& p, ArrayXd const& q) const;
};

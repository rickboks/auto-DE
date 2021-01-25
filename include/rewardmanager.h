#pragma once
#include <functional>
#include <map>
#include <string>
#include "Eigen/Dense"

using Eigen::VectorXd, Eigen::VectorXi;

class RewardManager {
	protected:
		VectorXd average(std::vector<std::vector<double>>const& deltas) const;
		VectorXd extreme(std::vector<std::vector<double>>const& deltas) const;
		std::vector<std::vector<double>> group(VectorXd const& improvements, 
				VectorXi const& assignment) const;
		int const K;
	public:
		static std::function<RewardManager*(int const)> create(std::string const id);
		RewardManager(int const K);
		virtual ~RewardManager (){};
		virtual VectorXd getReward(VectorXd const& improvements, 
				VectorXi const& assignment) const=0;
};

class AverageNormalizedReward : public RewardManager {
	public:
		AverageNormalizedReward(int const K): RewardManager(K){};
		VectorXd getReward(VectorXd const& improvements, VectorXi const& assignment) const;
};

class AverageReward : public RewardManager {
	public:
		AverageReward(int const K): RewardManager(K){};
		VectorXd getReward(VectorXd const& improvements, VectorXi const& assignment) const;
};

class ExtremeNormalizedReward : public RewardManager {
	public:
		ExtremeNormalizedReward(int const K): RewardManager(K){};
		VectorXd getReward(VectorXd const& improvements, VectorXi const& assignment) const;
};

class ExtremeReward : public RewardManager {
	public:
		ExtremeReward(int const K): RewardManager(K){};
		VectorXd getReward(VectorXd const& improvements, VectorXi const& assignment) const;
};

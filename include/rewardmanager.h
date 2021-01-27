#pragma once
#include <functional>
#include <map>
#include <string>
#include "Eigen/Dense"

using Eigen::ArrayXd, Eigen::ArrayXi;

class RewardManager {
	protected:
		ArrayXd average(std::vector<std::vector<double>>const& deltas) const;
		ArrayXd extreme(std::vector<std::vector<double>>const& deltas) const;
		ArrayXd normalized(ArrayXd const& x) const;
		std::vector<std::vector<double>> group(ArrayXd const& improvements, 
				ArrayXi const& assignment) const;
		int const K;
	public:
		static std::function<RewardManager*(int const)> create(std::string const id);
		RewardManager(int const K);
		virtual ~RewardManager (){};
		virtual ArrayXd getReward(ArrayXd const& improvements, 
				ArrayXi const& assignment) const=0;
};

class AverageNormalizedReward : public RewardManager {
	public:
		AverageNormalizedReward(int const K): RewardManager(K){};
		ArrayXd getReward(ArrayXd const& improvements, ArrayXi const& assignment) const;
};

class AverageReward : public RewardManager {
	public:
		AverageReward(int const K): RewardManager(K){};
		ArrayXd getReward(ArrayXd const& improvements, ArrayXi const& assignment) const;
};

class ExtremeNormalizedReward : public RewardManager {
	public:
		ExtremeNormalizedReward(int const K): RewardManager(K){};
		ArrayXd getReward(ArrayXd const& improvements, ArrayXi const& assignment) const;
};

class ExtremeReward : public RewardManager {
	public:
		ExtremeReward(int const K): RewardManager(K){};
		ArrayXd getReward(ArrayXd const& improvements, ArrayXi const& assignment) const;
};

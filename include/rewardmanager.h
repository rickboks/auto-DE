#pragma once
#include <functional>
#include <map>
#include <string>

class RewardManager {
	protected:
		std::vector<double> average(std::vector<std::vector<double>>const& deltas) const;
		std::vector<double> extreme(std::vector<std::vector<double>>const& deltas) const;
		std::vector<std::vector<double>> group(std::vector<double>const& improvements, 
				std::vector<int>const& assignment) const;
		int const K;
	public:
		static std::function<RewardManager*(int const)> create(std::string const id);
		RewardManager(int const K);
		virtual ~RewardManager (){};
		virtual std::vector<double> getReward(std::vector<double>const& improvements, 
				std::vector<int>const& assignment) const=0;
};

class AverageNormalizedReward : public RewardManager {
	public:
		AverageNormalizedReward(int const K): RewardManager(K){};
		std::vector<double> getReward(std::vector<double>const& improvements, std::vector<int>const& assignment) const;
};

class AverageReward : public RewardManager {
	public:
		AverageReward(int const K): RewardManager(K){};
		std::vector<double> getReward(std::vector<double>const& improvements, std::vector<int>const& assignment) const;
};

class ExtremeNormalizedReward : public RewardManager {
	public:
		ExtremeNormalizedReward(int const K): RewardManager(K){};
		std::vector<double> getReward(std::vector<double>const& improvements, std::vector<int>const& assignment) const;
};

class ExtremeReward : public RewardManager {
	public:
		ExtremeReward(int const K): RewardManager(K){};
		std::vector<double> getReward(std::vector<double>const& improvements, std::vector<int>const& assignment) const;
};

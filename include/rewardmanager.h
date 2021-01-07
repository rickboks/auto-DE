#pragma once
#include <functional>
#include <map>
#include <string>

class RewardManager {
	public:
		//RewardManager ();
		virtual ~RewardManager (){};
		virtual std::vector<double> getReward(std::vector<std::vector<double>>const& deltas) const=0;
};

extern std::map<std::string, std::function<RewardManager*()>> const rewardManagers;

class AverageNormalizedReward : public RewardManager {
	public:
		std::vector<double> getReward(std::vector<std::vector<double>>const& deltas) const;
};

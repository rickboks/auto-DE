#pragma once
#include <functional>
#include <map>
#include <string>

class RewardManager {
	protected:
		std::vector<double> average(std::vector<std::vector<double>>const& deltas) const;
		std::vector<double> extreme(std::vector<std::vector<double>>const& deltas) const;
		std::vector<double> normalized(std::vector<double> r) const;
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

class AverageAbsoluteReward : public RewardManager {
	public:
		std::vector<double> getReward(std::vector<std::vector<double>>const& deltas) const;
};

class ExtremeAbsoluteReward : public RewardManager {
	public:
		std::vector<double> getReward(std::vector<std::vector<double>>const& deltas) const;
};

class ExtremeNormalizedReward : public RewardManager {
	public:
		std::vector<double> getReward(std::vector<std::vector<double>>const& deltas) const;
};

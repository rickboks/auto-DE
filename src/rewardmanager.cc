#include "rewardmanager.h"
#include <algorithm>
#include <numeric>
#include "util.h"

std::function<RewardManager*(int const)> RewardManager::create(std::string const id){
#define LC(X) [](int const K){return new X(K);}
	if (id == "AN") return LC(AverageNormalizedReward);
	if (id == "AA") return LC(AverageReward);
	if (id == "EN") return LC(ExtremeNormalizedReward);
	if (id == "EA") return LC(ExtremeReward);
	throw std::invalid_argument("no such RewardManager: " + id);
}

RewardManager::RewardManager(int const K): K(K){}

std::vector<std::vector<double>> RewardManager::group(std::vector<double>const& improvements, 
		std::vector<int>const& assignment) const{
	std::vector<std::vector<double>> groups(K);
	for (unsigned int i = 0; i < improvements.size(); i++){
		groups[assignment[i]].push_back(improvements[i]);
	}
	return groups;
}

std::vector<double> RewardManager::average(std::vector<std::vector<double>>const& deltas) const {
	std::vector<double> r(deltas.size(), 0.);
	for (unsigned int i = 0; i < deltas.size(); i++){
		if (!deltas[i].empty())
			r[i] = mean(deltas[i]);
	}
	return r;
}

std::vector<double> RewardManager::extreme(std::vector<std::vector<double>>const& deltas) const {
	std::vector<double> r(deltas.size(), 0.);
	for (unsigned int i = 0; i < deltas.size(); i++){
		if (!deltas[i].empty())
			r[i] = *std::max_element(deltas[i].begin(), deltas[i].end());
	}
	return r;
}

std::vector<double> AverageNormalizedReward::getReward(std::vector<double>const& improvements, 
		std::vector<int>const& assignment) const {
	return normalize(average(group(improvements, assignment)));
}

std::vector<double> ExtremeNormalizedReward::getReward(std::vector<double>const& improvements, 
		std::vector<int>const& assignment) const {
	return normalize(extreme(group(improvements, assignment)));
}

std::vector<double> ExtremeReward::getReward(std::vector<double>const& improvements, 
		std::vector<int>const& assignment) const {
	return extreme(group(improvements, assignment));
}

std::vector<double> AverageReward::getReward(std::vector<double>const& improvements, 
		std::vector<int>const& assignment) const {
	return average(group(improvements, assignment));
}

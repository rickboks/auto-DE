#include "rewardmanager.h"
#include <algorithm>
#include <numeric>
#include "util.h"

std::function<RewardManager*(int const)> RewardManager::create(std::string const id){
#define ALIAS(X,Y) if (id==X) return [](int const K){return new Y(K);};
	ALIAS("AN", AverageNormalizedReward)
	ALIAS("AA", AverageReward)
	ALIAS("EN", ExtremeNormalizedReward)
	ALIAS("EA", ExtremeReward)
	throw std::invalid_argument("no such RewardManager: " + id);
}

RewardManager::RewardManager(int const K): K(K){}

std::vector<std::vector<double>> RewardManager::group(VectorXd const& improvements, 
		VectorXi const& assignment) const{
	std::vector<std::vector<double>> groups(K);
	for (unsigned int i = 0; i < improvements.size(); i++)
		groups[assignment(i)].push_back(improvements(i));
	return groups;
}

VectorXd RewardManager::average(std::vector<std::vector<double>>const& deltas) const {
	return VectorXd::NullaryExpr(K, [deltas](Eigen::Index const i){
			return (deltas[i].empty() ? 0. : VectorXd::Map(deltas[i].data(), deltas[i].size()).mean());
		});
}

VectorXd RewardManager::extreme(std::vector<std::vector<double>>const& deltas) const {
	return VectorXd::NullaryExpr(K, [deltas](Eigen::Index const i){
			return VectorXd::Map(deltas[i].data(), deltas[i].size()).maxCoeff();
		});
}

VectorXd RewardManager::normalized(VectorXd const& x) const {
	return x / x.maxCoeff();
}

VectorXd AverageNormalizedReward::getReward(VectorXd const& improvements, 
		VectorXi const& assignment) const {
	return normalized(average(group(improvements, assignment)));
}

VectorXd ExtremeNormalizedReward::getReward(VectorXd const& improvements, 
		VectorXi const& assignment) const {
	return normalized(extreme(group(improvements, assignment)));
}

VectorXd ExtremeReward::getReward(VectorXd const& improvements, 
		VectorXi const& assignment) const {
	return extreme(group(improvements, assignment));
}

VectorXd AverageReward::getReward(VectorXd const& improvements, 
		VectorXi const& assignment) const {
	return average(group(improvements, assignment));
}

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

std::vector<std::vector<double>> RewardManager::group(ArrayXd const& improvements, 
		ArrayXi const& assignment) const{
	std::vector<std::vector<double>> groups(K);
	for (unsigned int i = 0; i < improvements.size(); i++)
		groups[assignment(i)].push_back(improvements(i));
	return groups;
}

ArrayXd RewardManager::average(std::vector<std::vector<double>>const& deltas) const {
	return ArrayXd::NullaryExpr(K, [deltas](Eigen::Index const i){
			return (deltas[i].empty() ? 0. : ArrayXd::Map(deltas[i].data(), deltas[i].size()).mean());
		});
}

ArrayXd RewardManager::extreme(std::vector<std::vector<double>>const& deltas) const {
	return ArrayXd::NullaryExpr(K, [deltas](Eigen::Index const i){
			return ArrayXd::Map(deltas[i].data(), deltas[i].size()).maxCoeff();
		});
}

ArrayXd RewardManager::normalized(ArrayXd const& x) const {
	return x / x.maxCoeff();
}

ArrayXd AverageNormalizedReward::getReward(ArrayXd const& improvements, 
		ArrayXi const& assignment) const {
	return normalized(average(group(improvements, assignment)));
}

ArrayXd ExtremeNormalizedReward::getReward(ArrayXd const& improvements, 
		ArrayXi const& assignment) const {
	return normalized(extreme(group(improvements, assignment)));
}

ArrayXd ExtremeReward::getReward(ArrayXd const& improvements, 
		ArrayXi const& assignment) const {
	return extreme(group(improvements, assignment));
}

ArrayXd AverageReward::getReward(ArrayXd const& improvements, 
		ArrayXi const& assignment) const {
	return average(group(improvements, assignment));
}

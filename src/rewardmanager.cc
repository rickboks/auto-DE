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
	ALIAS("ER", ExtremeRankReward)
	ALIAS("AR", AverageRankReward)
	throw std::invalid_argument("no such RewardManager: " + id);
}

RewardManager::RewardManager(int const K): K(K){}

std::vector<std::vector<double>> RewardManager::group(ArrayXd const& credits, 
		ArrayXi const& assignment) const{
	std::vector<std::vector<double>> groups(K);
	for (unsigned int i = 0; i < credits.size(); i++){
		if (credits(i) > 0.)
			groups[assignment(i)].push_back(credits(i));
	}
	return groups;
}

ArrayXd RewardManager::average(std::vector<std::vector<double>>const& credits) const {
	return ArrayXd::NullaryExpr(K, [credits](Eigen::Index const i){
			return credits[i].empty() ? 0. : ArrayXd::Map(credits[i].data(), credits[i].size()).mean();
		});
}

ArrayXd RewardManager::extreme(std::vector<std::vector<double>>const& credits) const {
	return ArrayXd::NullaryExpr(K, [credits](Eigen::Index const i){
			return credits[i].empty() ? 0. : ArrayXd::Map(credits[i].data(), credits[i].size()).maxCoeff();
		});
}

ArrayXd RewardManager::normalized(ArrayXd const& x) const {
	double const max = x.maxCoeff();
	return max > 0. ? x/max : x;
}

ArrayXd RewardManager::ranks(ArrayXd const& x) const{
	ArrayXi ranks = ArrayXi::NullaryExpr(x.size(), [](Eigen::Index const i){return i;});
	std::sort(ranks.begin(), ranks.end(), [x](int const i, int const j){
		return x[i] < x[j];
	});

	int cRank = 0;
	ArrayXd score(ranks.size());
	for (int i = 0; i < ranks.size(); i++){
		if (i > 0 ? x[ranks[i]] > x[ranks[i-1]] : x[ranks[i]] > 0.)
			cRank++;
		score[ranks[i]] = cRank;
	}

	return score;
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

ArrayXd AverageRankReward::getReward(ArrayXd const& improvements, ArrayXi const& assignment) const{
	return average(group(ranks(improvements), assignment));
}

ArrayXd ExtremeRankReward::getReward(ArrayXd const& improvements, ArrayXi const& assignment) const{
	return extreme(group(ranks(improvements), assignment));
}

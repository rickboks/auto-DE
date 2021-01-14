#include "rewardmanager.h"
#include <algorithm>
#include <numeric>

#define LC(X) [](){return new X();}
std::map<std::string, std::function<RewardManager*()>> const rewardManagers ({
	{"AN", LC(AverageNormalizedReward)},
	{"AA", LC(AverageAbsoluteReward)},
	{"EA", LC(ExtremeAbsoluteReward)},
	{"EN", LC(ExtremeNormalizedReward)}
});

std::vector<double> RewardManager::average(std::vector<std::vector<double>>const& deltas) const {
	std::vector<double> r(deltas.size(), 0.);
	for (unsigned int i = 0; i < deltas.size(); i++){
		if (!deltas[i].empty())
			r[i] = std::accumulate(deltas[i].begin(), deltas[i].end(), 0.) / deltas[i].size();
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

std::vector<double> RewardManager::normalized(std::vector<double>r) const{
	double const maxR = *std::max_element(r.begin(), r.end());
	if (maxR > 0.)
		std::transform(r.begin(), r.end(), r.begin(), [maxR](double const& x){return x/maxR;});
	return r;
}

std::vector<double> AverageNormalizedReward::getReward(std::vector<std::vector<double>>const& deltas) const{
	return normalized(average(deltas));
}

std::vector<double> AverageAbsoluteReward::getReward(std::vector<std::vector<double>>const& deltas) const{
	return average(deltas);
}

std::vector<double> ExtremeAbsoluteReward::getReward(std::vector<std::vector<double>>const& deltas) const{
	return extreme(deltas);
}

std::vector<double> ExtremeNormalizedReward::getReward(std::vector<std::vector<double>>const& deltas) const{
	return normalized(extreme(deltas));
}

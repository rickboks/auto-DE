#include "rewardmanager.h"
#include <algorithm>
#include <numeric>

#define LC(X) [](){return new X();}
std::map<std::string, std::function<RewardManager*()>> const rewardManagers ({
	{"AN", LC(AverageNormalizedReward)}	
});

std::vector<double> AverageNormalizedReward::getReward(std::vector<std::vector<double>>const& deltas) const{
	std::vector<double> r(deltas.size());

	for (unsigned int i = 0; i < deltas.size(); i++){
		if (!deltas.empty())
			r[i] = std::accumulate(deltas[i].begin(), deltas[i].end(), 0.) / deltas[i].size();
		else 
			r[i] = 0.;
	}

	double const maxR = *std::max_element(r.begin(), r.end());
	if (maxR > 0.)
		std::transform(r.begin(), r.end(), r.begin(), [maxR](double const& x){return x/maxR;});

	return r;
}

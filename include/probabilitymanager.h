#include <functional>
#include <string>
#include <vector>
#include <map>

class ProbabilityManager {
	protected:
		int const K;
		double const pMin;
		double const pMax;
		double const beta;
	public:
		ProbabilityManager(int const K): K(K), pMin(.2/K), pMax(pMin + 1. - K * pMin), beta(.6){};
		virtual ~ProbabilityManager (){};
		virtual void updateProbability(std::vector<double>const& q, std::vector<double>& p) const=0;
};

extern std::map<std::string, std::function<ProbabilityManager*(int const)>> const probabilityManagers;

class AdaptivePursuitManager : public ProbabilityManager {
	public:
		AdaptivePursuitManager (int const K): ProbabilityManager(K){};
		void updateProbability(std::vector<double>const& q, std::vector<double>& p) const;
};

class ProbabilityMatchingManager : public ProbabilityManager {
	public:
		ProbabilityMatchingManager (int const K): ProbabilityManager(K){};
		void updateProbability(std::vector<double>const& q, std::vector<double>& p) const;
};

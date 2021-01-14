#include <functional>
#include <string>
#include <vector>
#include <map>

class ProbabilityManager {
	protected:
		int const K;
		double const pMin;
	public:
		ProbabilityManager(int const K): K(K), pMin(.2/K){};
		virtual ~ProbabilityManager (){};
		virtual void updateProbability(std::vector<double>const& q, std::vector<double>& p) const=0;
};

extern std::map<std::string, std::function<ProbabilityManager*(int const)>> const probabilityManagers;

class AdaptivePursuitManager : public ProbabilityManager {
	private:
		double const beta;
		double const pMax;
	public:
		AdaptivePursuitManager (int const K): ProbabilityManager(K), beta(.6), pMax(pMin + 1. - K * pMin){};
		void updateProbability(std::vector<double>const& q, std::vector<double>& p) const;
};

class ProbabilityMatchingManager : public ProbabilityManager {
	public:
		ProbabilityMatchingManager (int const K): ProbabilityManager(K){};
		void updateProbability(std::vector<double>const& q, std::vector<double>& p) const;
};

#include <functional>
#include <string>
#include <vector>
#include <map>

class ProbabilityManager {
	protected:
		int const K;
		double const pMin;
	public:
		static std::function<ProbabilityManager* (int const)> create(std::string const id);
		ProbabilityManager(int const K): K(K), pMin(.2/K){};
		virtual ~ProbabilityManager (){};
		virtual void updateProbability(std::vector<double>const& q, std::vector<double>& p) const=0;
};

class AdaptivePursuitManager : public ProbabilityManager {
	private:
		double const beta;
		double const pMax;
	public:
		AdaptivePursuitManager (int const K): ProbabilityManager(K), beta(.8), pMax(pMin + 1. - K * pMin){};
		void updateProbability(std::vector<double>const& q, std::vector<double>& p) const;
};

class ProbabilityMatchingManager : public ProbabilityManager {
	public:
		ProbabilityMatchingManager (int const K): ProbabilityManager(K){};
		void updateProbability(std::vector<double>const& q, std::vector<double>& p) const;
};

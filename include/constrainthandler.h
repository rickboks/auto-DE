#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include <functional>

class Solution;

class ConstraintHandler {
	protected:
		std::vector<double> const lb;
		std::vector<double> const ub;
		int const D;
		int nCorrected;
		bool isFeasible(Solution const * const p) const;
	public:
		ConstraintHandler(std::vector<double> const lb, std::vector<double> const ub): lb(lb), ub(ub), D(lb.size()), nCorrected(0){};
		virtual ~ConstraintHandler(){};
		virtual bool resample(Solution* const p, int const resamples);
		virtual void penalize(Solution* const p){};
		int getCorrections() const;
};

class DEConstraintHandler : virtual public ConstraintHandler {
	public:
		DEConstraintHandler(std::vector<double>const lb,std::vector<double>const ub): ConstraintHandler(lb, ub){};
		virtual ~DEConstraintHandler(){};
		virtual void repairDE(Solution* const p, Solution const * const base, Solution const* const target){}; // DE constraint handler
		virtual void repair(Solution* const p){};// Generic constraint handler
};

extern std::map<std::string, std::function<DEConstraintHandler* (std::vector<double>, std::vector<double>)>> const deCHs;

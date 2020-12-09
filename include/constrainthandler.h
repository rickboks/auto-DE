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
		virtual void penalize(Solution* const /*p*/){};
		virtual void repair(Solution* const /*p*/, Solution const * const /*base*/, Solution const* const /*target*/){}; // DE constraint handler
		virtual void repair(Solution* const /*p*/){};// Generic constraint handler
		int getCorrections() const;
};

extern std::map<std::string, std::function<ConstraintHandler* 
	(std::vector<double>, std::vector<double>)>> const constraintHandlers;

// Differential Evolution
class RandBaseRepair : public ConstraintHandler {
	public:
		RandBaseRepair(std::vector<double> const lb, std::vector<double> const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class MidpointBaseRepair : public ConstraintHandler{
	public:
		MidpointBaseRepair(std::vector<double> const lb, std::vector<double> const ub): ConstraintHandler(lb,ub) {};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class MidpointTargetRepair : public ConstraintHandler {
	public:
		MidpointTargetRepair(std::vector<double> const lb, std::vector<double> const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const * const base, Solution const* const target);
};

class ProjectionMidpointRepair : public ConstraintHandler {
	public:
		ProjectionMidpointRepair(std::vector<double> const lb, std::vector<double> const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class ProjectionBaseRepair: public ConstraintHandler {
	public:
		ProjectionBaseRepair(std::vector<double> const lb, std::vector<double> const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class ConservatismRepair : public ConstraintHandler {
	public:
		ConservatismRepair(std::vector<double> const lb, std::vector<double> const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

// Generic
class ResamplingRepair : public ConstraintHandler {
	public:
		ResamplingRepair(std::vector<double> const lb, std::vector<double> const ub)
			:ConstraintHandler(lb,ub){};
		bool resample(Solution * const p, int const resamples);
};

class DeathPenalty : public ConstraintHandler {
	public:
		DeathPenalty(std::vector<double>const lb,std::vector<double>const ub):ConstraintHandler(lb,ub){};
		void penalize(Solution* const p);
};

class ReinitializationRepair : public ConstraintHandler {
	public:
		ReinitializationRepair(std::vector<double> const lb, std::vector<double> const ub)
			:ConstraintHandler(lb,ub){}; 
		void repair(Solution* const p);
};

class ProjectionRepair : public ConstraintHandler {
	public:
		ProjectionRepair(std::vector<double> const lb, std::vector<double> const ub)
			:ConstraintHandler(lb,ub){};
		void repair(Solution* const p);
};

class ReflectionRepair : public ConstraintHandler{
	public:
		ReflectionRepair(std::vector<double> const lb, std::vector<double> const ub)
			:ConstraintHandler(lb,ub){}; 
		void repair(Solution* const p);
};

class WrappingRepair : public ConstraintHandler {
	public:
		WrappingRepair(std::vector<double> const lb, std::vector<double> const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p);
};

class TransformationRepair : public ConstraintHandler { //Adapted from https://github.com/psbiomech/c-cmaes
	private:
		std::vector<double> al, au, xlo, xhi, r;
		bool shift(Solution* const p);
	public:
		TransformationRepair(std::vector<double> const lb, std::vector<double> const ub);
		void repair(Solution* const p);
};

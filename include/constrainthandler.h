#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include <functional>
#include "Eigen/Dense"

using Eigen::ArrayXd;
class Solution;

class ConstraintHandler {
	protected:
		ArrayXd const lb;
		ArrayXd const ub;
		int const D;
		int nCorrected;
	public:
		static std::function<ConstraintHandler* (ArrayXd const, ArrayXd const)> 
			create(std::string const id);
		ConstraintHandler(ArrayXd const lb, ArrayXd const ub): lb(lb), ub(ub), D(lb.size()), nCorrected(0){};
		virtual ~ConstraintHandler(){};
		virtual bool resample(Solution* const p, int const resamples);
		virtual void penalize(Solution* const /*p*/){};
		virtual void repair(Solution* const /*p*/, Solution const * const /*base*/, Solution const* const /*target*/){}; // DE constraint handler
		virtual void repair(Solution* const /*p*/){};// Generic constraint handler
		int getCorrections() const;
		bool isFeasible(Solution const * const p) const;
};

class RandBaseRepair : public ConstraintHandler {
	public:
		RandBaseRepair(ArrayXd const lb, ArrayXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class MidpointBaseRepair : public ConstraintHandler{
	public:
		MidpointBaseRepair(ArrayXd const lb, ArrayXd const ub): ConstraintHandler(lb,ub) {};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class MidpointTargetRepair : public ConstraintHandler {
	public:
		MidpointTargetRepair(ArrayXd const lb, ArrayXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const * const base, Solution const* const target);
};

class ProjectionMidpointRepair : public ConstraintHandler {
	public:
		ProjectionMidpointRepair(ArrayXd const lb, ArrayXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class ProjectionBaseRepair: public ConstraintHandler {
	public:
		ProjectionBaseRepair(ArrayXd const lb, ArrayXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class ConservatismRepair : public ConstraintHandler {
	public:
		ConservatismRepair(ArrayXd const lb, ArrayXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class ResamplingRepair : public ConstraintHandler {
	public:
		ResamplingRepair(ArrayXd const lb, ArrayXd const ub)
			:ConstraintHandler(lb,ub){};
		bool resample(Solution * const p, int const resamples);
};

class DeathPenalty : public ConstraintHandler {
	public:
		DeathPenalty(ArrayXd const lb,ArrayXd const ub):ConstraintHandler(lb,ub){};
		void penalize(Solution* const p);
};

class ReinitializationRepair : public ConstraintHandler {
	public:
		ReinitializationRepair(ArrayXd const lb, ArrayXd const ub)
			:ConstraintHandler(lb,ub){}; 
		void repair(Solution* const p);
};

class ProjectionRepair : public ConstraintHandler {
	public:
		ProjectionRepair(ArrayXd const lb, ArrayXd const ub)
			:ConstraintHandler(lb,ub){};
		void repair(Solution* const p);
};

class ReflectionRepair : public ConstraintHandler{
	public:
		ReflectionRepair(ArrayXd const lb, ArrayXd const ub)
			:ConstraintHandler(lb,ub){}; 
		void repair(Solution* const p);
};

class WrappingRepair : public ConstraintHandler {
	public:
		WrappingRepair(ArrayXd const lb, ArrayXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p);
};

class TransformationRepair : public ConstraintHandler { //Adapted from https://github.com/psbiomech/c-cmaes
	private:
		ArrayXd al, au, xlo, xhi, r;
		bool shift(Solution* const p);
	public:
		TransformationRepair(ArrayXd const lb, ArrayXd const ub);
		void repair(Solution* const p);
};

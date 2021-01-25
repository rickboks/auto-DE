#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include <functional>
#include "Eigen/Dense"

using Eigen::VectorXd;
class Solution;

class ConstraintHandler {
	protected:
		VectorXd const lb;
		VectorXd const ub;
		int const D;
		int nCorrected;
	public:
		static std::function<ConstraintHandler* (VectorXd const, VectorXd const)> 
			create(std::string const id);
		ConstraintHandler(VectorXd const lb, VectorXd const ub): lb(lb), ub(ub), D(lb.size()), nCorrected(0){};
		virtual ~ConstraintHandler(){};
		virtual bool resample(Solution* const p, int const resamples);
		virtual void penalize(Solution* const /*p*/){};
		virtual void repair(Solution* const /*p*/, Solution const * const /*base*/, Solution const* const /*target*/){}; // DE constraint handler
		virtual void repair(Solution* const /*p*/){};// Generic constraint handler
		int getCorrections() const;
		bool isFeasible(Solution const * const p) const;
};

// Differential Evolution
class RandBaseRepair : public ConstraintHandler {
	public:
		RandBaseRepair(VectorXd const lb, VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class MidpointBaseRepair : public ConstraintHandler{
	public:
		MidpointBaseRepair(VectorXd const lb, VectorXd const ub): ConstraintHandler(lb,ub) {};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class MidpointTargetRepair : public ConstraintHandler {
	public:
		MidpointTargetRepair(VectorXd const lb, VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const * const base, Solution const* const target);
};

class ProjectionMidpointRepair : public ConstraintHandler {
	public:
		ProjectionMidpointRepair(VectorXd const lb, VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class ProjectionBaseRepair: public ConstraintHandler {
	public:
		ProjectionBaseRepair(VectorXd const lb, VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class ConservatismRepair : public ConstraintHandler {
	public:
		ConservatismRepair(VectorXd const lb, VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

// Generic
class ResamplingRepair : public ConstraintHandler {
	public:
		ResamplingRepair(VectorXd const lb, VectorXd const ub)
			:ConstraintHandler(lb,ub){};
		bool resample(Solution * const p, int const resamples);
};

class DeathPenalty : public ConstraintHandler {
	public:
		DeathPenalty(VectorXd const lb,VectorXd const ub):ConstraintHandler(lb,ub){};
		void penalize(Solution* const p);
};

class ReinitializationRepair : public ConstraintHandler {
	public:
		ReinitializationRepair(VectorXd const lb, VectorXd const ub)
			:ConstraintHandler(lb,ub){}; 
		void repair(Solution* const p);
};

class ProjectionRepair : public ConstraintHandler {
	public:
		ProjectionRepair(VectorXd const lb, VectorXd const ub)
			:ConstraintHandler(lb,ub){};
		void repair(Solution* const p);
};

class ReflectionRepair : public ConstraintHandler{
	public:
		ReflectionRepair(VectorXd const lb, VectorXd const ub)
			:ConstraintHandler(lb,ub){}; 
		void repair(Solution* const p);
};

class WrappingRepair : public ConstraintHandler {
	public:
		WrappingRepair(VectorXd const lb, VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p);
};

class TransformationRepair : public ConstraintHandler { //Adapted from https://github.com/psbiomech/c-cmaes
	private:
		VectorXd al, au, xlo, xhi, r;
		bool shift(Solution* const p);
	public:
		TransformationRepair(VectorXd const lb, VectorXd const ub);
		void repair(Solution* const p);
};

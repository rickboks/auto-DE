#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include <functional>
#include "Eigen/Dense"

class Solution;

class ConstraintHandler {
	protected:
		Eigen::VectorXd const lb;
		Eigen::VectorXd const ub;
		int const D;
		int nCorrected;
		bool isFeasible(Solution const * const p) const;
	public:
		static std::function<ConstraintHandler* (Eigen::VectorXd const, Eigen::VectorXd const)> 
			create(std::string const id);
		ConstraintHandler(Eigen::VectorXd const lb, Eigen::VectorXd const ub): lb(lb), ub(ub), D(lb.size()), nCorrected(0){};
		virtual ~ConstraintHandler(){};
		virtual bool resample(Solution* const p, int const resamples);
		virtual void penalize(Solution* const /*p*/){};
		virtual void repair(Solution* const /*p*/, Solution const * const /*base*/, Solution const* const /*target*/){}; // DE constraint handler
		virtual void repair(Solution* const /*p*/){};// Generic constraint handler
		int getCorrections() const;
};

// Differential Evolution
class RandBaseRepair : public ConstraintHandler {
	public:
		RandBaseRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class MidpointBaseRepair : public ConstraintHandler{
	public:
		MidpointBaseRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub): ConstraintHandler(lb,ub) {};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class MidpointTargetRepair : public ConstraintHandler {
	public:
		MidpointTargetRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const * const base, Solution const* const target);
};

class ProjectionMidpointRepair : public ConstraintHandler {
	public:
		ProjectionMidpointRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class ProjectionBaseRepair: public ConstraintHandler {
	public:
		ProjectionBaseRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

class ConservatismRepair : public ConstraintHandler {
	public:
		ConservatismRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p, Solution const* const base, Solution const* const target);
};

// Generic
class ResamplingRepair : public ConstraintHandler {
	public:
		ResamplingRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub)
			:ConstraintHandler(lb,ub){};
		bool resample(Solution * const p, int const resamples);
};

class DeathPenalty : public ConstraintHandler {
	public:
		DeathPenalty(Eigen::VectorXd const lb,Eigen::VectorXd const ub):ConstraintHandler(lb,ub){};
		void penalize(Solution* const p);
};

class ReinitializationRepair : public ConstraintHandler {
	public:
		ReinitializationRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub)
			:ConstraintHandler(lb,ub){}; 
		void repair(Solution* const p);
};

class ProjectionRepair : public ConstraintHandler {
	public:
		ProjectionRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub)
			:ConstraintHandler(lb,ub){};
		void repair(Solution* const p);
};

class ReflectionRepair : public ConstraintHandler{
	public:
		ReflectionRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub)
			:ConstraintHandler(lb,ub){}; 
		void repair(Solution* const p);
};

class WrappingRepair : public ConstraintHandler {
	public:
		WrappingRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub):ConstraintHandler(lb,ub){};
		void repair(Solution* const p);
};

class TransformationRepair : public ConstraintHandler { //Adapted from https://github.com/psbiomech/c-cmaes
	private:
		Eigen::VectorXd al, au, xlo, xhi, r;
		bool shift(Solution* const p);
	public:
		TransformationRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub);
		void repair(Solution* const p);
};

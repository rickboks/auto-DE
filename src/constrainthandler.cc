#include "constrainthandler.h"
#include "util.h"
#include "solution.h"
#include <stdexcept>

std::function<ConstraintHandler* (VectorXd const, VectorXd const)> 
ConstraintHandler::create(std::string const id){
#define ALIAS(X,Y) if(id==X) return [](VectorXd const lb, VectorXd const ub){return new Y(lb,ub);};
	ALIAS("DP", DeathPenalty)
	ALIAS("RS", ResamplingRepair)
	ALIAS("RI", ReinitializationRepair)
	ALIAS("PR", ProjectionRepair)
	ALIAS("RF", ReflectionRepair)
	ALIAS("WR", WrappingRepair)
	ALIAS("TR", TransformationRepair)
	ALIAS("RB", RandBaseRepair)
	ALIAS("MB", MidpointBaseRepair)
	ALIAS("MT", MidpointTargetRepair)
	ALIAS("PM", ProjectionMidpointRepair)
	ALIAS("PB", ProjectionBaseRepair)
	ALIAS("CO", ConservatismRepair)
	throw std::invalid_argument("no such ConstraintHandler: " + id);
}

bool ConstraintHandler::isFeasible(Solution const * const p) const{
	for (int i = 0; i < D; i++){
		if (p->X(i) < lb(i) || p->X(i) > ub(i) )
			return false;
	}
	return true;
}

bool ConstraintHandler::resample(Solution * const /*p*/, int const /*resamples*/){
	return false;
}

int ConstraintHandler::getCorrections() const {
	return nCorrected;
}

// Differential Evolution
void RandBaseRepair::repair(Solution* const p, Solution const* const base, Solution const* const /*target*/) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->X(i) > ub(i)){
			p->setX(i, base->X(i) + rng.randDouble(0,1) * (ub(i) - base->X(i)));
			repaired=true;
		} else if (p->X(i) < lb(i)){
			p->setX(i, base->X(i) + rng.randDouble(0,1) * (lb(i) - base->X(i)));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

void MidpointBaseRepair::repair(Solution* const p, Solution const* const base, Solution const* const /*target*/) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->X(i) > ub(i)){
			p->setX(i, 0.5 * (base->X(i) + ub(i)));
			repaired=true;
		} else if (p->X(i) < lb(i)){
			p->setX(i, 0.5 * (base->X(i) + lb(i)));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

void MidpointTargetRepair::repair(Solution* const p, Solution const* const /*base*/, Solution const* const target) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->X(i) > ub(i)){
			p->setX(i, 0.5 * (target->X(i) + ub(i)));
			repaired=true;
		} else if (p->X(i) < lb(i)){
			p->setX(i, 0.5 * (target->X(i) + lb(i)));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

void ProjectionMidpointRepair::repair(Solution* const p, Solution const* const /*base*/, Solution const* const /*target*/) {
	VectorXd const x = p->X();
	std::vector<double>alphas(D+1);
	alphas[D] = 1.;

	for (int i = 0; i < D; i++){
		if (x(i) > ub(i)){
			alphas[i] = (lb(i) - ub(i))/(lb(i) - 2. * x(i) + ub(i));
		} else if (x(i) < lb(i)){
			alphas[i] = (ub(i) - lb(i))/(lb(i) - 2. * x(i) + ub(i));
		} else
			alphas[i] = std::numeric_limits<double>::max(); 
	}

	auto alpha=std::min_element(alphas.begin(), alphas.end());
	if (alpha != std::next(alphas.end(), -1)){
		p->setX((1. - *alpha) * (lb + ub) / 2. + *alpha * x);
		nCorrected++;
	}

	p->setX(p->X().cwiseMax(lb).cwiseMin(ub)); // clamp
}

void ProjectionBaseRepair::repair(Solution* const p, Solution const* const base, Solution const* const /*target*/) {
	VectorXd const x = p->X();
	std::vector<double> alphas(D+1);
	alphas[D] = 1.;

	for (int i = 0; i < D; i++){
		if (x(i) > ub(i) && x(i) - base->X(i) > 1.0e-12){
			alphas[i] = (ub(i) - base->X(i)) / (x(i) - base->X(i));
		} else if (x(i) < lb(i) && base->X(i) - x(i) > 1.0e-12){
			alphas[i] = (base->X(i) - lb(i)) / (base->X(i) - x(i));
		} else
			alphas[i] = std::numeric_limits<double>::max(); 
	}

	auto alpha=std::min_element(alphas.begin(), alphas.end());
	if (alpha != std::next(alphas.end(), -1)){
		p->setX((1.- *alpha) * base->X() + *alpha * x);
		nCorrected++;
	}

	p->setX(p->X().cwiseMax(lb).cwiseMin(ub)); // clamp
}

void ConservatismRepair::repair(Solution* const p, Solution const*const base, Solution const*const /*target*/){
	if (!isFeasible(p)){
		p->copy(base);
		nCorrected++;
	}
}

// Generic
bool ResamplingRepair::resample(Solution * const p, int const resamples) {
	if (isFeasible(p)){
		return false;
	} else if (resamples >= 100){
		p->setX(p->X().cwiseMax(lb).cwiseMin(ub)); // clamp
		return false;
	}

	if (resamples == 0) nCorrected++; // Only count the first resample

	return true;
}

void DeathPenalty::penalize(Solution* const p) {
	if (!isFeasible(p)){
		p->setFitness(std::numeric_limits<double>::max());
		nCorrected++;
	}
}

// Reinitialization
void ReinitializationRepair::repair(Solution* const p) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->X(i) < lb(i) || p->X(i) > ub(i)){
			p->setX(i, rng.randDouble(lb(i), ub(i)));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

// Projection
void ProjectionRepair::repair(Solution* const p) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->X(i) < lb(i)){
			p->setX(i, lb(i));
			repaired=true;
		} else if (p->X(i) > ub(i)){
			p->setX(i, ub(i));
			repaired=true;
		}
	}

	if (repaired) nCorrected++;
}

// Reflection
void ReflectionRepair::repair(Solution* const p) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		while (true){
			if (p->X(i) < lb(i)){
				p->setX(i, 2. * lb(i) - p->X(i));
				repaired=true;
			}
			else if (p->X(i) > ub(i)){
				p->setX(i, 2. * ub(i) - p->X(i));
				repaired=true;
			} else
				break;
		}
	}
	if (repaired) nCorrected++;
}

// Wrapping
void WrappingRepair::repair(Solution* const p) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->X(i) < lb(i)){
			p->setX(i, ub(i) - std::fmod(lb(i) - p->X(i), std::abs(ub(i)-lb(i))));
			repaired=true;
		} else if (p->X(i) > ub(i)) {
			p->setX(i, lb(i) + std::fmod(p->X(i) - ub(i), std::abs(ub(i)-lb(i))));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

// Transformation, adapted from https://github.com/psbiomech/c-cmaes
TransformationRepair::TransformationRepair(VectorXd const lb, VectorXd const ub) 
	:ConstraintHandler(lb,ub), al(D), au(D), xlo(D), xhi(D), r(D){
	for (int i = 0; i < D; i++){
		al(i) = std::min( (ub(i)-lb(i))/2., (1.+std::abs(lb(i)))/20. );
		au(i) = std::min( (ub(i)-lb(i))/2., (1.+std::abs(ub(i)))/20. );
		xlo(i) = lb(i) - 2. * al(i) - (ub(i) - lb(i)) / 2.;
		xhi(i) = ub(i) + 2. * au(i) + (ub(i) - lb(i)) / 2.;
		r(i) = 2.*(ub(i) - lb(i) + al(i) + au(i));
	}
}

void TransformationRepair::repair(Solution* const p) {
	bool repaired = shift(p);
	for (int i = 0; i < D; i++){
		double const x_i = p->X(i);
		if (x_i < lb(i) + al(i)){
			p->setX(i, lb(i) + pow(x_i - (lb(i) - al(i)),2.)/(4.*al(i)));
			repaired=true;
		}
		else if (x_i > ub(i)-au(i)){
			p->setX(i, ub(i) - pow(x_i - (ub(i) + au(i)),2.)/(4.*au(i)));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

bool TransformationRepair::shift(Solution* const p){
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->X(i) < xlo(i)) {
			p->setX(i, p->X(i) + r(i) * (1 + (int)((xlo(i) - p->X(i))/r(i))));
			repaired=true;
		}
		if (p->X(i) > xhi(i)){
			p->setX(i, p->X(i) - r(i) * (1 + (int)((p->X(i) - xhi(i))/r(i))));
			repaired=true;
		}

		if (p->X(i) < lb(i) - al(i)){
			p->setX(i, p->X(i) + 2. * (lb(i) - al(i) - p->X(i)));
			repaired=true;
		}
		if (p->X(i) > ub(i) + au(i)){
			p->setX(i, p->X(i) - 2. * (p->X(i) - ub(i) - au(i)));
			repaired=true;
		}
	}
	return repaired;
}

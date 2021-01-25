#include "constrainthandler.h"
#include "util.h"
#include "solution.h"
#include <stdexcept>

std::function<ConstraintHandler* (Eigen::VectorXd const, Eigen::VectorXd const)> 
ConstraintHandler::create(std::string const id){
#define LC(X) [](Eigen::VectorXd const lb, Eigen::VectorXd const ub){return new X(lb,ub);}
	// Generic
	if (id == "DP") return LC(DeathPenalty);
	if (id == "RS") return LC(ResamplingRepair);

	// Almost generic
	if (id == "RI") return LC(ReinitializationRepair);
	if (id == "PR") return LC(ProjectionRepair);
	if (id == "RF") return LC(ReflectionRepair);
	if (id == "WR") return LC(WrappingRepair);
	if (id == "TR") return LC(TransformationRepair);

	//// DE
	if (id == "RB") return LC(RandBaseRepair);
	if (id == "MB") return LC(MidpointBaseRepair);
	if (id == "MT") return LC(MidpointTargetRepair);
	if (id == "PM") return LC(ProjectionMidpointRepair);
	if (id == "PB") return LC(ProjectionBaseRepair);
	if (id == "CO") return LC(ConservatismRepair);
	throw std::invalid_argument("no such ConstraintHandler: " + id);
}

bool ConstraintHandler::isFeasible(Solution const * const p) const{
	for (int i = 0; i < D; i++){
		if (p->getX(i) < lb[i] - 1.0e-12 || p->getX(i) > ub[i] + 1.0e-12){
			return false;
		}
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
		if (p->getX(i) > ub[i]){
			p->setX(i, base->getX(i) + rng.randDouble(0,1) * (ub[i] - base->getX(i)));
			repaired=true;
		} else if (p->getX(i) < lb[i]){
			p->setX(i, base->getX(i) + rng.randDouble(0,1) * (lb[i] - base->getX(i)));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

void MidpointBaseRepair::repair(Solution* const p, Solution const* const base, Solution const* const /*target*/) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->getX(i) > ub[i]){
			p->setX(i, 0.5 * (base->getX(i) + ub[i]));
			repaired=true;
		} else if (p->getX(i) < lb[i]){
			p->setX(i, 0.5 * (base->getX(i) + lb[i]));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

void MidpointTargetRepair::repair(Solution* const p, Solution const* const /*base*/, Solution const* const target) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->getX(i) > ub[i]){
			p->setX(i, 0.5 * (target->getX(i) + ub[i]));
			repaired=true;
		} else if (p->getX(i) < lb[i]){
			p->setX(i, 0.5 * (target->getX(i) + lb[i]));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

void ProjectionMidpointRepair::repair(Solution* const p, Solution const* const /*base*/, Solution const* const /*target*/) {
	Eigen::VectorXd x = p->getX();
	std::vector<double>alphas(D+1);
	alphas[D] = 1.;

	for (int i = 0; i < D; i++){
		if (x[i] > ub[i]){
			alphas[i] = (lb[i] - ub[i])/(lb[i] - 2. * x[i] + ub[i]);
		} else if (x[i] < lb[i]){
			alphas[i] = (ub[i] - lb[i])/(lb[i] - 2. * x[i] + ub[i]);
		} else
			alphas[i] = std::numeric_limits<double>::max(); 
	}

	std::vector<double>::iterator alpha=std::min_element(alphas.begin(), alphas.end());
	if (alpha != std::next(alphas.end(), -1)){
		x = (1. - *alpha) * (lb + ub) / 2 + *alpha * x;
		p->setX(x);
		nCorrected++;
	}
}

void ProjectionBaseRepair::repair(Solution* const p, Solution const* const base, Solution const* const /*target*/) {
	Eigen::VectorXd const x = p->getX();
	std::vector<double> alphas(D+1);
	alphas[D] = 1.;

	for (int i = 0; i < D; i++){
		if (x[i] > ub[i] && x[i] - base->getX(i) > 1.0e-12){
			alphas[i] = (ub[i] - base->getX(i)) / (x[i] - base->getX(i));
		} else if (x[i] < lb[i] && base->getX(i) - x[i] > 1.0e-12){
			alphas[i] = (base->getX(i) - lb[i]) / (base->getX(i) - x[i]);
		} else
			alphas[i] = std::numeric_limits<double>::max(); 
	}

	std::vector<double>::iterator alpha=std::min_element(alphas.begin(), alphas.end());
	if (alpha != std::next(alphas.end(), -1)){
		p->setX((1.- *alpha) * base->getX() + *alpha * x);
		nCorrected++;
	}

	p->setX(p->getX().cwiseMax(lb).cwiseMin(ub));
}

void ConservatismRepair::repair(Solution* const p, Solution const*const base, Solution const*const /*target*/){
	if (!isFeasible(p)){
		p->copy(base);
		nCorrected++;
	}
}

//Adapted from https://github.com/psbiomech/c-cmaes
// Generic
bool ResamplingRepair::resample(Solution * const p, int const resamples) {
	if (isFeasible(p)){
		return false;
	} else if (resamples >= 100){
		p->setX(p->getX().cwiseMax(lb).cwiseMin(ub));
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
		if (p->getX(i) < lb[i] || p->getX(i) > ub[i]){
			p->setX(i, rng.randDouble(lb[i], ub[i]));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

// Projection
void ProjectionRepair::repair(Solution* const p) {
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->getX(i) < lb[i]){
			p->setX(i, lb[i]);
			repaired=true;
		} else if (p->getX(i) > ub[i]){
			p->setX(i, ub[i]);
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
			if (p->getX(i) < lb[i]){
				p->setX(i, 2. * lb[i] - p->getX(i));
				repaired=true;
			}
			else if (p->getX(i) > ub[i]){
				p->setX(i, 2. * ub[i] - p->getX(i));
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
		if (p->getX(i) < lb[i]){
			p->setX(i, ub[i] - std::fmod(lb[i] - p->getX(i), std::abs(ub[i]-lb[i])));
			repaired=true;
		} else if (p->getX(i) > ub[i]) {
			p->setX(i, lb[i] + std::fmod(p->getX(i) - ub[i], std::abs(ub[i]-lb[i])));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

// Transformation, adapted from https://github.com/psbiomech/c-cmaes
TransformationRepair::TransformationRepair(Eigen::VectorXd const lb, Eigen::VectorXd const ub) 
	:ConstraintHandler(lb,ub), al(D), au(D), xlo(D), xhi(D), r(D){
	for (int i = 0; i < D; i++){
		al[i] = std::min( (ub[i]-lb[i])/2., (1.+std::abs(lb[i]))/20. );
		au[i] = std::min( (ub[i]-lb[i])/2., (1.+std::abs(ub[i]))/20. );
		xlo[i] = lb[i] - 2. * al[i] - (ub[i] - lb[i]) / 2.;
		xhi[i] = ub[i] + 2. * au[i] + (ub[i] - lb[i]) / 2.;
		r[i] = 2.*(ub[i] - lb[i] + al[i] + au[i]);
	}
}

void TransformationRepair::repair(Solution* const p) {
	bool repaired = shift(p);
	for (int i = 0; i < D; i++){
		double const x_i = p->getX(i);
		if (x_i < lb[i] + al[i]){
			p->setX(i, lb[i] + pow(x_i - (lb[i] - al[i]),2.)/(4.*al[i]));
			repaired=true;
		}
		else if (x_i > ub[i]-au[i]){
			p->setX(i, ub[i] - pow(x_i - (ub[i] + au[i]),2.)/(4.*au[i]));
			repaired=true;
		}
	}
	if (repaired) nCorrected++;
}

bool TransformationRepair::shift(Solution* const p){
	bool repaired = false;
	for (int i = 0; i < D; i++){
		if (p->getX(i) < xlo[i]) {
			p->setX(i, p->getX(i) + r[i] * (1 + (int)((xlo[i] - p->getX(i))/r[i])));
			repaired=true;
		}
		if (p->getX(i) > xhi[i]){
			p->setX(i, p->getX(i) - r[i] * (1 + (int)((p->getX(i) - xhi[i])/r[i])));
			repaired=true;
		}

		if (p->getX(i) < lb[i] - al[i]){
			p->setX(i, p->getX(i) + 2. * (lb[i] - al[i] - p->getX(i)));
			repaired=true;
		}
		if (p->getX(i) > ub[i] + au[i]){
			p->setX(i, p->getX(i) - 2. * (p->getX(i) - ub[i] - au[i]));
			repaired=true;
		}
	}
	return repaired;
}

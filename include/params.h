#pragma once
#include "Eigen/Dense"
#include "rng.h"

constexpr double pi() { return std::atan(1)*4.; }

namespace params {
	// Logging
	extern bool log_activations;
	extern int log_activations_interval;
	extern bool log_parameters;
	extern int log_parameters_interval;
	extern bool log_positions;
	extern int log_positions_interval;
	extern bool log_diversity;
	extern int log_diversity_interval;
	extern bool log_repairs;
	extern int log_repairs_interval;
	extern std::string extra_data_path;
	extern Eigen::IOFormat const vecFmt; 

	// DE
	extern double F;
	extern double Cr;
	extern double popsize_multiplier;
	extern double convergence_delta;
	extern bool restart_on_convergence;

	extern double CO_omega;

	// Quality
	extern double WS_alpha;

	// Probability
	extern double AP_beta;
	extern double PM_AP_pMin_divider;
	extern std::function<double(int)> const PM_AP_pMin;
	extern std::function<double(int)> const AP_pMax;

	// Parameter self-adaptation
	extern std::function<int(int, int)>	const SHADE_H;
	extern std::function<int(int)> const SHADE_p;

	// Constraint handling
	extern int RS_max_resamples;
};

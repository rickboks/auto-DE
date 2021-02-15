#pragma once
#include "Eigen/Dense"
#include "rng.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-const-variable"

constexpr double pi() { return std::atan(1)*4.; }

namespace params {
	// Logging
	static bool const log_activations = true;
	static int const log_activations_interval = 1;
	static bool const log_parameters = true;
	static int const log_parameters_interval = 5;
	static bool const log_diversity = true;
	static int const log_diversity_interval = 5;
	static std::string const extra_data_path = "./extra_data";
	static Eigen::IOFormat const vecFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, " ", "", "", "", "", "");

	// DE
	static double const F = 0.5;
	static double const Cr = 0.9;
	static double const popsize_multiplier = 5;
	static double const convergence_delta = 1e-9;

	static double const CO_omega = pi()/4.;

	// Quality
	static double const WS_alpha = 0.4;

	// Probability
	static double const AP_beta = 0.6;
	static auto const PM_AP_pMin = [](int const K) -> double {return 1. / (3. * (K-1.));};
	static auto const AP_pMax = [](int const K) -> double {return 1. - (K-1.) * params::PM_AP_pMin(K);};

	// Parameter self-adaptation
	static auto	const SHADE_H = [](int const popSize, int const K) -> int {return std::max(popSize/K, 10);};
	static auto const SHADE_p = [](int const popSize) -> int {return rng.randInt(0, rng.randInt(2, popSize/5)-1);};

	// Constraint handling
	static int const RS_max_resamples = 100;
};

#pragma GCC diagnostic pop

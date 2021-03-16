// Logging
#include "params.h"
bool params::log_activations = false;
int params::log_activations_interval = 10;
bool params::log_parameters = false;
int params::log_parameters_interval = 10;
bool params::log_diversity = false;
int params::log_diversity_interval = 10;
bool params::log_positions = false;
int params::log_positions_interval = 1;
std::string const params::extra_data_path = "./extra_data";
Eigen::IOFormat const params::vecFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, " ", "", "", "", "", "");

// DE
double const params::F = 0.5;
double const params::Cr = 0.9;
double params::popsize_multiplier = 5;
double const params::convergence_delta = 1e-9;
bool params::restart_on_convergence = true;

double const params::CO_omega = pi()/4.;

// Quality
double params::WS_alpha = 0.4;

// Probability
double params::AP_beta = 0.6;
double params::PM_AP_pMin_divider = 3;
std::function<double(int)> const params::PM_AP_pMin =
  [](int const K) -> double {return 1. / (params::PM_AP_pMin_divider * (K-1.));};
std::function<double(int)> const params::AP_pMax = 
  [](int const K) -> double {return 1. - (K-1.) * params::PM_AP_pMin(K);};

// Parameter self-adaptation
std::function<int(int, int)> const params::SHADE_H = 
  [](int const popSize, int const K) -> int {return std::max(popSize/K, 10);};
std::function<int(int)> const params::SHADE_p = 
  [](int const popSize) -> int {return rng.randInt(0, rng.randInt(2, popSize/5)-1);};

// Constraint handling
int const params::RS_max_resamples = 100;

#include "rng.h"
#include <random>

double RNG::randDouble(double const start, double const end){
	std::uniform_real_distribution<double> dist(start, end);
	return dist(rng);
}

int RNG::randInt(int const start, int const end){
	std::uniform_int_distribution<int> dist(start, end);
	return dist(rng);
}

double RNG::normalDouble(double const mean, double const stdDev){
	std::normal_distribution<double> N(mean, stdDev);
	return N(rng);
}

double RNG::cauchyDouble(double const a, double const b){
	std::cauchy_distribution<double> C(a,b);
	return C(rng);
}

RNG rng; //Global random number generator

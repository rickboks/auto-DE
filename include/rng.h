#pragma once
#include <random>
#include <algorithm>

class RNG {
	private:
		std::random_device dev;
		std::mt19937 rng;
	public:
		RNG() : rng(dev()){} 
		double randDouble(double const start, double const end);
		int randInt(int const start, int const end);
		double normalDistribution(double const mean, double const stdDev);
		double cauchyDistribution(double const a, double const b);
};

extern RNG rng;

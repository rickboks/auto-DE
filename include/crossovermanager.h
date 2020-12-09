#pragma once
#include <random>
#include <vector>
#include <algorithm>
#include <functional>
#include "solution.h"

class CrossoverManager {
	public:
		CrossoverManager(){};
		virtual ~CrossoverManager(){};

		virtual Solution* crossover(Solution const* const target, 
				Solution const* const donor, double const Cr) const = 0;
};

extern std::map<std::string, std::function<CrossoverManager* ()>> const crossovers;

class BinomialCrossoverManager : public CrossoverManager {
	public:
		BinomialCrossoverManager(){};
		Solution* crossover(Solution const* const target, Solution const* const donor, double const Cr) const;
};

class ExponentialCrossoverManager : public CrossoverManager {
	public:
		ExponentialCrossoverManager(){};
		Solution* crossover(Solution const* const target, 
				Solution const* const donor, double const Cr) const;
};

class ArithmeticCrossoverManager : public CrossoverManager {
	public:
		ArithmeticCrossoverManager(){};
		Solution* crossover(Solution const* const target, 
				Solution const* const donor, double const Cr) const;
};

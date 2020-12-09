#pragma once
#include <random>
#include <vector>
#include <algorithm>
#include <functional>
#include "solution.h"

class CrossoverManager {
	protected:
		int const D;
	public:
		CrossoverManager(int const D);
		virtual ~CrossoverManager();

		virtual Solution* crossover(Solution const* const target, 
				Solution const* const donor, double const Cr) const = 0;
};

extern std::map<std::string, std::function<CrossoverManager* (int const)>> const crossovers;

class BinomialCrossoverManager : public CrossoverManager {
	public:
		BinomialCrossoverManager(int const D): CrossoverManager(D){};
		Solution* crossover(Solution const* const target, Solution const* const donor, double const Cr) const;
};

class ExponentialCrossoverManager : public CrossoverManager {
	public:
		ExponentialCrossoverManager(int const D): CrossoverManager(D){};
		Solution* crossover(Solution const* const target, 
				Solution const* const donor, double const Cr) const;
};

class ArithmeticCrossoverManager : public CrossoverManager {
	public:
		ArithmeticCrossoverManager(int const D): CrossoverManager(D){};
		Solution* crossover(Solution const* const target, 
				Solution const* const donor, double const Cr) const;
};

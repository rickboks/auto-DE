#pragma once
#include "solution.h"

class CrossoverManager {
	public:
		static std::function<CrossoverManager* ()> create(std::string const id);
		CrossoverManager(){};
		virtual ~CrossoverManager(){};
		virtual Solution* crossover(Solution const* const target, 
				Solution const* const donor, double const Cr) const = 0;
};

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

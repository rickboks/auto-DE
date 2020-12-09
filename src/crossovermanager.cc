#include "util.h"
#include "crossovermanager.h"

CrossoverManager::CrossoverManager(int const D): D(D){}
CrossoverManager::~CrossoverManager(){}

#define LC(X) [](int const D){return new X(D);}
std::map<std::string, std::function<CrossoverManager* (int const)>> const crossovers({
		{"B", LC(BinomialCrossoverManager)},
		{"E", LC(ExponentialCrossoverManager)},
		{"A", LC(ArithmeticCrossoverManager)},
});

Solution* BinomialCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const Cr) const{
	std::vector<double> x(D);

	int const jrand = rng.randInt(0,D-1);
	for (int j = 0; j < D; j++){
		if (j == jrand || rng.randDouble(0,1) < Cr){
			x[j] = donor->getX(j);
		} else {
			x[j] = target->getX(j); 
		}
	}
	return new Solution(x);
}

Solution* ExponentialCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const Cr) const{
	std::vector<double> x(D);
	int const start = rng.randInt(0,D-1);

	int L = 0;
	do {
		L++;
	} while (rng.randDouble(0,1) <= Cr && L <= D);

	int const end = (start+L-1) % D;
	for (int i = 0; i < D; i++){
		if ((end >= start && (i >= start && i <= end)) || (start > end && (i <= end || i >= start)))
			x[i] = donor->getX(i);
		else 
			x[i] = target->getX(i);
	}
	return new Solution(x);
}

Solution* ArithmeticCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const Cr) const{	
	std::vector<double> x = target->getX();
	double const k = rng.randDouble(0,1);

	std::vector<double> subtraction(D);
	subtract(donor->getX(), target->getX(), subtraction);
	scale(subtraction, k);
	add(x, subtraction, x);
	return new Solution(x);
}

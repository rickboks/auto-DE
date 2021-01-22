#include "util.h"
#include "crossovermanager.h"

std::function<CrossoverManager* ()> CrossoverManager::create(std::string const id){
#define LC(X) [](){return new X();}
	if (id == "B") return LC(BinomialCrossoverManager);
	if (id == "E") return LC(ExponentialCrossoverManager);
	if (id == "A") return LC(ArithmeticCrossoverManager);
	return NULL;
}

Solution* BinomialCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const Cr) const{
	std::vector<double> x = target->getX();
	unsigned int const jrand = rng.randInt(0,x.size()-1);
	for (unsigned int j = 0; j < x.size(); j++){
		if (j == jrand || rng.randDouble(0,1) < Cr)
			x[j] = donor->getX(j);
	}
	return new Solution(x);
}

Solution* ExponentialCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const Cr) const{
	std::vector<double> x = target->getX();
	unsigned int const start = rng.randInt(0,x.size()-1);

	unsigned int L = 1;
	while (rng.randDouble(0,1) <= Cr && L <= x.size())
		L++;

	unsigned int const end = (start+L-1) % x.size();
	for (unsigned int i = 0; i < x.size(); i++){
		if ((end >= start && (i >= start && i <= end)) || (start > end && (i <= end || i >= start)))
			x[i] = donor->getX(i);
	}
	return new Solution(x);
}

Solution* ArithmeticCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const /*Cr*/) const{	
	std::vector<double> x = target->getX();
	double const k = rng.randDouble(0,1);

	std::vector<double> subtraction = subtract(donor->getX(), target->getX());
	subtraction = scale(subtraction, k);
	x = add(x, subtraction);
	return new Solution(x);
}

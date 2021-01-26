#include "util.h"
#include "crossovermanager.h"
#include <algorithm>
#include <functional>

std::function<CrossoverManager* ()> CrossoverManager::create(std::string const id){
#define LC(X) [](){return new X();}
	if (id == "B") return LC(BinomialCrossoverManager);
	if (id == "E") return LC(ExponentialCrossoverManager);
	if (id == "A") return LC(ArithmeticCrossoverManager);
	throw std::invalid_argument("no such CrossoverManager: " + id);
}

Solution* BinomialCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const Cr) const{
	VectorXd x = target->getX();
	unsigned int const jrand = rng.randInt(0,x.size()-1);
	for (unsigned int j = 0; j < x.size(); j++){
		if (j == jrand || rng.randDouble(0,1) < Cr)
			x[j] = donor->getX(j);
	}
	return new Solution(x);
}

Solution* ExponentialCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const Cr) const{
	VectorXd x = target->getX();
	int const start = rng.randInt(0,x.size()-1);

	int L = 1;
	while (L < x.size() && rng.randDouble(0,1) <= Cr)
		L++;

	int const end = (start+L-1) % x.size();

	auto const condition = end >= start ? \
		[](int const i, int const start, int const end) {return i >= start && i <= end;}: 
		[](int const i, int const start, int const end) {return i <= end || i >= start;};

	for (unsigned int i = 0; i < x.size(); i++)
		if (condition(i, start, end))
			x[i] = donor->getX(i);

	return new Solution(x);
}

Solution* ArithmeticCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const /*Cr*/) const{	
	return new Solution(target->getX() + rng.randDouble(0,1) * (donor->getX() - target->getX()));
}

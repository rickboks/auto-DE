#include "util.h"
#include "crossovermanager.h"

std::function<CrossoverManager* ()> CrossoverManager::create(std::string const id){
#define ALIAS(X, Y) if(id == X) return [](){return new Y();};
	ALIAS("B", BinomialCrossoverManager)
	ALIAS("E", ExponentialCrossoverManager)
	ALIAS("A", ArithmeticCrossoverManager)
	throw std::invalid_argument("no such CrossoverManager: " + id);
}

Solution* BinomialCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const Cr) const{
	ArrayXd x = target->X();
	unsigned int const jrand = rng.randInt(0,x.size()-1);
	for (unsigned int j = 0; j < x.size(); j++){
		if (j == jrand || rng.randDouble(0,1) < Cr)
			x(j) = donor->X(j);
	}
	return new Solution(x);
}

Solution* ExponentialCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const Cr) const{
	ArrayXd x = target->X();
	int const start = rng.randInt(0,x.size()-1);

	int L = 1;
	while (L < x.size() && rng.randDouble(0,1) < Cr)
		L++;

	int const end = (start+L-1) % x.size();

	auto const condition = end >= start ? \
		[](int const i, int const start, int const end) {return i >= start && i <= end;}: 
		[](int const i, int const start, int const end) {return i <= end || i >= start;};

	for (unsigned int i = 0; i < x.size(); i++)
		if (condition(i, start, end))
			x(i) = donor->X(i);

	return new Solution(x);
}

Solution* ArithmeticCrossoverManager::crossover(Solution const* const target, Solution const* const donor, double const /*Cr*/) const{	
	return new Solution(target->X() + rng.randDouble(0,1) * (donor->X() - target->X()));
}

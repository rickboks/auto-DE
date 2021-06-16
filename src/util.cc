#include "util.h"
#include "rng.h"

double distance(Solution const*const s1, Solution const*const s2) {
	return distance(s1->X(), s2->X());
}

double distance(ArrayXd const& s1, ArrayXd const& s2){
	return (s1 - s2).matrix().norm();
}

std::vector<int> range(int const size){
	std::vector<int> r(size);
	std::iota(r.begin(), r.end(), 0);
	return r;
}

Solution* getBest(std::vector<Solution*>const& genomes){
	return *std::min_element(genomes.begin(), genomes.end(), 
			[](Solution const* const a, Solution const* const b){return *a < *b;});
}

Solution* getWorst(std::vector<Solution*>const& genomes){
	return *std::max_element(genomes.begin(), genomes.end(), 
			[](Solution const * const a, Solution const * const b){return *a < *b;});
}

std::vector<Solution*> sortOnFitness(std::vector<Solution*> genomes){
	std::sort(genomes.begin(), genomes.end(), [](Solution* a, Solution* b){return *a < *b;});
	return genomes;
}

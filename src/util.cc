#include "util.h"
#include "rng.h"
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <numeric>

std::string generateConfig(std::string const templateFile, std::string const name){
	std::string const folder = "configurations";
	std::string const cfgFile = folder + "/" + name + ".ini";

	if (!std::experimental::filesystem::exists(folder)){
		std::cerr << "Creating directory \"" << folder << "\"."<< std::endl;
		std::experimental::filesystem::create_directory(folder);
	}

	std::ifstream src(templateFile, std::ios::binary);
    std::ofstream dst(cfgFile, std::ios::binary);

	dst << src.rdbuf() 
		<< "result_folder = " + name << std::endl 
		<< "algorithm_name = " + name << std::endl;

	return cfgFile;
}

double distance(Solution const*const s1, Solution const*const s2) {
	return distance(s1->X(), s2->X());
}

double distance(ArrayXd const& s1, ArrayXd const& s2){
	return (s1 - s2).matrix().norm();
}

std::string checkFilename(std::string const fn){
	std::string newFn = fn;
	int i = 1;
	while (std::experimental::filesystem::exists(newFn)){
		newFn = fn + "." + std::to_string(i);
		i++;
	}
	return newFn;
}

std::vector<int> range(int const size){
	std::vector<int> r(size);
	std::iota(r.begin(), r.end(), 0);
	return r;
}

Solution* getPBest(std::vector<Solution*>const& genomes, bool const isSorted){
	int const P = rng.randDouble(2./genomes.size(), .2) * genomes.size(); // The number of elements to consider
	if (isSorted)
		return P > 0 ? genomes[rng.randInt(0, P-1)] : genomes[0];
	else {
		std::vector<Solution*> const sorted = sortOnFitness(genomes);
		return P > 0 ? sorted[rng.randInt(0, P-1)] : sorted[0];
	}
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

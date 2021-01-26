#include "util.h"
#include "rng.h"
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <numeric>

VectorXd normalizeAbs(VectorXd vec){ // Normalize by largest *absolute* value
	double const absMax = vec.cwiseAbs().maxCoeff();
	if (absMax > 0.) 
		vec /= absMax;
	return vec;
}

int argmax(VectorXd const& x){
	double max = x(0);
	int idx = 0;
	for (int i = 1; i < x.size(); i++){
		if (x(i) > max){
			max = x(i);
			idx=i;
		}
	}
	return idx;
}

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

double distance(VectorXd const& s1, VectorXd const& s2){
	return (s1 - s2).norm();
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

Solution* getPBest(std::vector<Solution*>const& genomes){
	int const P = rng.randDouble(2./genomes.size(), .2) * genomes.size(); // The number of elements to consider
	std::vector<Solution*> const sorted = sortOnFitness(genomes);
	return (P > 0 ? sorted[rng.randInt(0, P-1)] : sorted[0]);
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

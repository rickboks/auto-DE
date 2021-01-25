#include "util.h"
#include "rng.h"
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <numeric>

VectorXd normalize(VectorXd vec){
	double const absMax = vec.cwiseAbs().maxCoeff();
	if (absMax != 0.)
		vec /= absMax;
	return vec;
}

double mean (std::vector<double>const& vec){
	return std::accumulate(vec.begin(), vec.end(), 0.) / vec.size();
}

bool comparePtrs(Solution const* const a, Solution const *const b){
	return *a < *b;
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
	Eigen::VectorXd const diff = (s1->getX() - s2->getX());
	return std::sqrt(std::sqrt((diff.array() * diff.array()).sum())); 
}

double distance(Eigen::VectorXd const& s1, Eigen::VectorXd const& s2){
	Eigen::VectorXd const diff = (s1 - s2);
	return std::sqrt(std::sqrt((diff.array() * diff.array()).sum())); 
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

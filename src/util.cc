#include "util.h"
#include "rng.h"
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <numeric>

std::vector<double> scale(std::vector<double> x, double const scalar){
	std::transform(x.begin(), x.end(), x.begin(), [scalar](double const& x){return x*scalar;});
	return x;
}

std::vector<double> add(std::vector<double> lhs, std::vector<double>const rhs){
	std::transform(lhs.begin(), lhs.end(), rhs.begin(), lhs.begin(), std::plus<double>());
	return lhs;
}

std::vector<double> subtract(std::vector<double> lhs, std::vector<double>const rhs){
	std::transform( lhs.begin(), lhs.end(), rhs.begin(), lhs.begin(), std::minus<double>());
	return lhs;
}

std::vector<double> randomMult(std::vector<double> vec, double const min, double const max){
	for (unsigned int i = 0; i < vec.size(); i++)
		vec[i] *= rng.randDouble(min, max);
	return vec;
}

std::vector<double> normalize(std::vector<double>vec){
	double const absMax = std::abs(*std::max_element(vec.begin(), vec.end(), 
			[](double const& x, double const& y){return std::abs(x) < std::abs(y);}));

	if (absMax != 0.)
		std::for_each(vec.begin(), vec.end(), [absMax](double& x){x/=absMax;});
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
	double d=0;
	for (int i = 0; i < s1->D; i++){
		d += std::pow(s1->getX(i) - s2->getX(i), 2.); 
	}
	return std::sqrt(d);
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

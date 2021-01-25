#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include "rng.h"
#include "solution.h"

[[nodiscard]] std::vector<double> scale(std::vector<double> vec, double const x);
[[nodiscard]] std::vector<double> divide(std::vector<double> vec, double const x);
[[nodiscard]] std::vector<double> add(std::vector<double> lhs, std::vector<double>const rhs);
[[nodiscard]] std::vector<double> subtract(std::vector<double> lhs, std::vector<double>const rhs);
[[nodiscard]] std::vector<double> randomMult(std::vector<double> vec, double const min, double const max);
[[nodiscard]] std::vector<double> normalize(std::vector<double> vec);
[[nodiscard]] double mean(std::vector<double>const& vec);
bool comparePtrs(Solution const* const a, Solution const* const b);
double distance(Solution const* const s1, Solution const* const s2);
double distance(Eigen::VectorXd const& s1, Eigen::VectorXd const& s2);
std::string generateConfig(std::string const templateFile, std::string const name);
std::string checkFilename(std::string const fn);
std::vector<int> range(int const size);

template <typename T>
std::vector<T> vectorize(T const * const arr, int const size){
	std::vector<T> vec(size);
	for (int i = 0; i < size; i++)
		vec[i] = arr[i];
	return vec;
}

template <typename T>
std::vector<T> remove(std::vector<T> vec, int const i){
	vec.erase(vec.begin()+i);
	return vec;
}

template <typename T>
void sortOnFitness(std::vector<T*>& genomes){
	std::sort(genomes.begin(), genomes.end(), comparePtrs);
}

template<typename T>
T* getPBest(std::vector<T*> genomes){
	double const p = std::max(0.05, 3./genomes.size());
	sortOnFitness(genomes);
	std::vector<T*> bestP = std::vector<T*>(genomes.begin(), genomes.begin() + (genomes.size() * p));

	if (bestP.empty())
		return genomes[0];
	else
		return bestP[rng.randInt(0, bestP.size()-1)];
}

template<typename T>
T* getBest(std::vector<T*>const& genomes){
	T* best = NULL;

	for (T* s : genomes)
		if (best == NULL || *s < *best)
			best = s;

	return best;
}

template<typename T>
T* getWorst(std::vector<T*>const& genomes){
	T* worst = NULL;

	for (T* s : genomes)
		if (worst == NULL || *worst < *s)
			worst = s;

	return worst;
}

template<typename T>
std::vector<T*> pickRandom(std::vector<T*> possibilities, int const n){
	std::vector<T*> picked;
	for (int i = 0; i < n; i++){
		int const r = rng.randInt(0,possibilities.size()-1);
		T* x = possibilities[r];
		possibilities.erase(possibilities.begin() + r);
		picked.push(x);
	}
	return picked;
}

template<typename T>
std::vector<T> rouletteSelect(std::vector<T> possibilities, std::vector<double> prob, int const n, bool const replace){
	std::vector<T> particles;
	double totalProb = std::accumulate(prob.begin(), prob.end(), 0.);

	for (int i = 0; i < n; i++){
		double rand = rng.randDouble(0.,totalProb);
		for (unsigned int i = 0; i < possibilities.size(); i++){
			rand -= prob[i];
			if (rand <= 0.){
				T selected = possibilities[i];
				if (!replace){
					possibilities.erase(possibilities.begin() + i);
					totalProb -= prob[i];
					prob.erase(prob.begin() + i);
				}
				particles.push_back(selected);
			} 
		}
	}
	return particles;
}

template <typename T>
void printVec(std::vector<T> const v){
	for (double d : v)
		std::cout << d << " ";
	std::cout << std::endl;
}


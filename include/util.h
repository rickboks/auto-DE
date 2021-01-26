#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include "rng.h"
#include "solution.h"

double distance(Solution const* const s1, Solution const* const s2);
double distance(VectorXd const& s1, VectorXd const& s2);
std::string generateConfig(std::string const templateFile, std::string const name);
std::string checkFilename(std::string const fn);
std::vector<int> range(int const size);
Solution* getPBest(std::vector<Solution*> genomes);
Solution* getBest(std::vector<Solution*>const& genomes);
Solution* getWorst(std::vector<Solution*>const& genomes);
void sortOnFitness(std::vector<Solution*>& genomes);

template <typename T>
std::vector<T> remove(std::vector<T> vec, int const i){
	vec.erase(vec.begin()+i);
	return vec;
}

template<typename T>
std::vector<T> pickRandom(std::vector<T> possibilities, int const n){
	std::vector<T> picked;
	picked.reserve(n);
	for (int i = 0; i < n; i++){
		int const r = rng.randInt(0,possibilities.size()-1);
		T const x = possibilities[r];
		possibilities.erase(possibilities.begin() + r);
		picked.push_back(x);
	}
	return picked;
}

template<typename T>
std::vector<T> rouletteSelect(std::vector<T> possibilities, std::vector<double> prob, int const n, bool const replace){
	std::vector<T> picked;
	picked.reserve(n);
	double totalProb = std::accumulate(prob.begin(), prob.end(), 0.);
	for (int i = 0; i < n; i++){
		double rand = rng.randDouble(0.,totalProb);
		for (unsigned int j = 0; j < possibilities.size(); j++){
			rand -= prob[j];
			if (rand <= 0.){
				T const t = possibilities[j];
				if (!replace){
					possibilities.erase(possibilities.begin() + j);
					totalProb -= prob[j];
					prob.erase(prob.begin() + j);
				}
				picked.push_back(t);
				break;
			} 
		}
	}
	return picked;
}

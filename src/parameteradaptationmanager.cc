#include <iostream>
#include <numeric>
#include <stdexcept>
#include "parameteradaptationmanager.h"
#include "rng.h"

std::function<ParameterAdaptationManager*(std::vector<Solution*>, int const)> ParameterAdaptationManager::create(std::string const id){
#define LC(X) [](std::vector<Solution*>const& population, int const K){return new X(population, K);}
	if (id == "S") return LC(SHADEManager);
	if (id == "C") return LC(ConstantParameterManager);
	throw std::invalid_argument("no such ParameterAdaptationManager: " + id);
}

ParameterAdaptationManager::ParameterAdaptationManager(std::vector<Solution*>const& population, int const K)
	: popSize(population.size()), K(K){
}

// SHADE
SHADEManager::SHADEManager(std::vector<Solution*>const& population, int const K) : 
	ParameterAdaptationManager(population, K), H(popSize/K), MCr(K, H), MF(K, H), 
	k(VectorXi::Zero(K)){

	MCr.fill(.5);
	MF.fill(.5);
}

double SHADEManager::weightedMean(VectorXd const& x, VectorXd const& w) const{
	return (x.array() * w.array()).sum();
}

double SHADEManager::weightedLehmerMean(VectorXd const& x, VectorXd const& w) const{
	return (w.array() * x.array() * x.array()).sum() / (w.array() * x.array()).sum();
}

VectorXd SHADEManager::w(VectorXd const& improvements) const {
	return improvements/improvements.sum();
}

void SHADEManager::update(VectorXd const& improvement){
	std::vector<std::vector<double>> SF(K), SCr(K), improvements(K);

	for (int i = 0; i < popSize; i++){
		int const c = previousAssignment[i];
		if (improvement[i] > 0.){
			SF[c].push_back(previousFs[i]); 
			SCr[c].push_back(previousCrs[i]);
			improvements[c].push_back(improvement[i]);
		}
	}
	
	for (int c = 0; c < K; c++){
		if (!SF[c].empty()){
			VectorXd const _w = w(VectorXd::Map(improvements[c].data(), improvements[c].size()));
			MF(c,k[c]) = weightedLehmerMean(VectorXd::Map(SF[c].data(), SF[c].size()), _w);
			MCr(c,k[c]) = weightedMean(VectorXd::Map(SCr[c].data(), SF[c].size()), _w);
			k[c] = (k[c]+1)%H;
		}
	}
}

void SHADEManager::nextParameters(VectorXd& Fs, VectorXd& Crs, VectorXi const& assignment){
	for (int i = 0; i < popSize; i++){
		int const randIndex = rng.randInt(0, H-1);
		int const config = assignment[i]; // The configuration that this individual will use

		// Update mutation rate
		double const MFr = MF(config, randIndex);
		do{
			Fs[i] = std::min(rng.cauchyDistribution(MFr, .1), 1.);
		} while (Fs[i] <= 0.);

		// Update crossover rate
		double const MCrr = MCr(config, randIndex);
		Crs[i] = std::clamp(rng.normalDistribution(MCrr, .1),0.,1.);
	}

	previousFs = Fs;
	previousCrs = Crs;
	previousAssignment = assignment;
}

//NO ADAPTATION
ConstantParameterManager::ConstantParameterManager(std::vector<Solution*>const& population, int const K)
 : ParameterAdaptationManager(population, K), F(.5), Cr(.9){}

void ConstantParameterManager::update(VectorXd const& /*trials*/){
	//ignore
}

void ConstantParameterManager::nextParameters(VectorXd& Fs, VectorXd& Crs, 
		VectorXi const& /*assignment*/){
	std::fill(Fs.begin(), Fs.end(), F);
	std::fill(Crs.begin(), Crs.end(), Cr);
}

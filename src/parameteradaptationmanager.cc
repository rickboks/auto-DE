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
	ParameterAdaptationManager(population, K), H(popSize/K), MCr(K, std::vector<double>(H)), MF(K, std::vector<double>(H)), 
	k(K, 0){

	for (std::vector<double>& v : MCr)
		std::fill(v.begin(), v.end(), .5);

	for (std::vector<double>& v : MF)
		std::fill(v.begin(), v.end(), .5);
}

double SHADEManager::weightedMean(std::vector<double>const& x, std::vector<double>const& w) const{
	double sum = 0;
	for (unsigned int i = 0; i < x.size(); i++)
		sum += w[i] * x[i];
	return sum;
}

double SHADEManager::weightedLehmerMean(std::vector<double>const& x, std::vector<double>const& w) const{
	double wSqSum=0, wSum=0;

	for (unsigned int i = 0; i < x.size(); i++){
		wSqSum += w[i]*(x[i]*x[i]);
		wSum += w[i]*x[i];
	}

	return wSqSum / wSum;
}

std::vector<double> SHADEManager::w(std::vector<double>const& improvements) const {
	std::vector<double> w(improvements.size());
	double sum = std::accumulate(improvements.begin(), improvements.end(), 0.);
	for (unsigned int i = 0; i < improvements.size(); i++)
		w[i] = improvements[i]/sum;
	return w;
}

void SHADEManager::update(std::vector<double>const& improvement){
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
			std::vector<double> const _w = w(improvements[c]);
			MF[c][k[c]] = weightedLehmerMean(SF[c], _w);
			MCr[c][k[c]] = weightedMean(SCr[c], _w);
			k[c] = (k[c]+1)%H;
		}
	}
}

void SHADEManager::nextParameters(std::vector<double>& Fs, std::vector<double>& Crs, std::vector<int>const& assignment){
	for (int i = 0; i < popSize; i++){
		int const randIndex = rng.randInt(0, H-1);
		int const config = assignment[i]; // The configuration that this individual will use

		// Update mutation rate
		double const MFr = MF[config][randIndex];
		do{
			Fs[i] = std::min(rng.cauchyDistribution(MFr, .1), 1.);
		} while (Fs[i] <= 0.);

		// Update crossover rate
		double const MCrr = MCr[config][randIndex];
		Crs[i] = std::clamp(rng.normalDistribution(MCrr, .1),0.,1.);
	}

	previousFs = Fs;
	previousCrs = Crs;
	previousAssignment = assignment;
}

//NO ADAPTATION
ConstantParameterManager::ConstantParameterManager(std::vector<Solution*>const& population, int const K)
 : ParameterAdaptationManager(population, K), F(.5), Cr(.9){}

void ConstantParameterManager::update(std::vector<double>const& /*trials*/){
	//ignore
}

void ConstantParameterManager::nextParameters(std::vector<double>& Fs, std::vector<double>& Crs, 
		std::vector<int>const& /*assignment*/){
	std::fill(Fs.begin(), Fs.end(), F);
	std::fill(Crs.begin(), Crs.end(), Cr);
}

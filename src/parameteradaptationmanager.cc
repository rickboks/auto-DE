#include <numeric>
#include "parameteradaptationmanager.h"
#include "rng.h"

#define LC(X) [](int const popSize){return new X(popSize);}
std::map<std::string, std::function<ParameterAdaptationManager*(int const)>> const parameterAdaptations({
		{"J", LC(JADEManager)},
		{"S", LC(SHADEManager)},
		{"C", LC(ConstantParameterManager)},
	});

	ParameterAdaptationManager::ParameterAdaptationManager(int const popSize): popSize(popSize){}

	//JADE
	JADEManager::JADEManager(int const popSize)
 : ParameterAdaptationManager(popSize), MuCr(0.5), MuF(0.6), c(0.1){}

void JADEManager::update(std::vector<double>const& orig, std::vector<double>const& trials){
	std::vector<double> SF, SCr;
	for (int i = 0; i < popSize; i++){
		if (trials[i] < orig[i]){
			SF.push_back(previousFs[i]); 
			SCr.push_back(previousCrs[i]);
		}
	}

	if (!SCr.empty()){
		MuCr = (1.0-c) * MuCr + c * (std::accumulate(SCr.begin(), SCr.end(), 0.0)/SCr.size());
		MuCr = std::min(std::max(MuCr, 0.01), 1.0);
	}

	if (!SF.empty()){
		MuF = (1.0-c) * MuF + c * lehmerMean(SF);
		MuF = std::min(std::max(MuF, 0.01), 1.2);
	}
}

void JADEManager::nextParameters(std::vector<double>& Crs, std::vector<double>& Fs){
	int third = popSize/3;
	std::vector<double> indices(popSize);
	std::iota(indices.begin(), indices.end(), 0);
	rng.shuffle(indices.begin(), indices.end());

	// Update mutation rate
	for (int i = 0; i < third; i++)
		Fs[indices[i]] = rng.randDouble(0.0,1.2);

	for (int i = third; i < popSize; i++){
		do {
			Fs[indices[i]] = std::min(rng.normalDistribution(MuF, 0.1),1.2);	
		} while ( Fs[indices[i]] <= 0. );
	}

	// Update crossover rate
	for (int i = 0; i < popSize; i++)
		Crs[i] = std::min(std::max(rng.normalDistribution(MuCr, 0.1),0.0),1.0);

	previousFs = Fs;
	previousCrs = Crs;
}

double JADEManager::lehmerMean(std::vector<double>const& SF) const {
	double sumOfSquares = std::inner_product(SF.begin(), SF.end(), SF.begin(), 0.0);
	double sum = std::accumulate(SF.begin(), SF.end(), 0.0);
	return sumOfSquares/sum;
}

// SHADE
SHADEManager::SHADEManager(int const popSize) : ParameterAdaptationManager(popSize), H(popSize), MCr(H), MF(H), k(0){
	std::fill(MCr.begin(), MCr.end(), 0.5);
	std::fill(MF.begin(), MF.end(), 0.5);
}

double SHADEManager::weightedMean(std::vector<double>const& x, std::vector<double>const& w)const{
	double sum = 0;
	for (unsigned int i = 0; i < x.size(); i++)
		sum += w[i] * x[i];
	return sum;
}

double SHADEManager::weightedLehmerMean(std::vector<double>const& x, std::vector<double>const& w)const{
	double wSqSum=0;
	double wSum=0;

	for (unsigned int i = 0; i < x.size(); i++){
		wSqSum += w[i]*(x[i]*x[i]);
		wSum += w[i]*x[i];
	}

	return wSqSum / wSum;
}

std::vector<double> SHADEManager::w(std::vector<double>const& delta) const {
	std::vector<double> w(delta.size());
	double sum = std::accumulate(delta.begin(), delta.end(), 0.);
	for (unsigned int i = 0; i < delta.size(); i++)
		w[i] = delta[i]/sum;
	return w;
}

void SHADEManager::update(std::vector<double>const& targets, std::vector<double>const& trials){
	std::vector<double> SF, SCr, delta;
	for (int i = 0; i < popSize; i++){
		if (trials[i] < targets[i]){
			SF.push_back(previousFs[i]); 
			SCr.push_back(previousCrs[i]);
			delta.push_back(std::abs(trials[i] - targets[i]));
		}
	}

	if (!SF.empty() && !MCr.empty()){
		std::vector<double> const _w = w(delta);
		MF[k] = weightedLehmerMean(SF, _w);
		MCr[k] = weightedMean(SCr, _w);
		k = (k+1)%H;
	}
}

void SHADEManager::nextParameters(std::vector<double>& Fs, std::vector<double>& Crs){
	for (int i = 0; i < popSize; i++){
		int const randIndex = rng.randInt(0, H-1);

		// Update mutation rate
		double const MFr = MF[randIndex];
		do{
			Fs[i] = std::min(rng.cauchyDistribution(MFr, 0.1), 1.);
		} while (Fs[i] <= 0.);

		// Update crossover rate
		double const MCrr = MCr[randIndex];
		Crs[i] = std::min(std::max(rng.normalDistribution(MCrr, 0.1),0.),1.);
	}
 
	previousFs = Fs;
	previousCrs = Crs;
}

//NO ADAPTATION
ConstantParameterManager::ConstantParameterManager(int const popSize)
 : ParameterAdaptationManager(popSize), F(0.5), Cr(.9){}

void ConstantParameterManager::update(std::vector<double>const& /*orig*/, std::vector<double>const& /*trials*/){
	//ignore
}

void ConstantParameterManager::nextParameters(std::vector<double>& Fs, std::vector<double>& Crs){
	// Update mutation rate
	std::fill(Fs.begin(), Fs.end(), F);
	// Update crossover rate
	std::fill(Crs.begin(), Crs.end(), Cr);
}

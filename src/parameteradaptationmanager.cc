#include <numeric>
#include "parameteradaptationmanager.h"
#include "rng.h"

#define LC(X) [](int const popSize, int const K){return new X(popSize, K);}
std::map<std::string, std::function<ParameterAdaptationManager*(int const, int const)>> const parameterAdaptations({
		//{"J", LC(JADEManager)},
		{"S", LC(SHADEManager)},
		{"C", LC(ConstantParameterManager)},
	});

ParameterAdaptationManager::ParameterAdaptationManager(int const popSize, int const K): popSize(popSize), K(K){}

////JADE
//JADEManager::JADEManager(int const popSize)
//: ParameterAdaptationManager(popSize), MuCr(.5), MuF(.6), c(.1){}

//void JADEManager::update(std::vector<double>const& orig, std::vector<double>const& trials){
	//std::vector<double> SF, SCr;
	//for (int i = 0; i < popSize; i++){
		//if (trials[i] < orig[i]){
			//SF.push_back(previousFs[i]); 
			//SCr.push_back(previousCrs[i]);
		//}
	//}

	//if (!SCr.empty()){
		//MuCr = (1.-c) * MuCr + c * (std::accumulate(SCr.begin(), SCr.end(), .0)/SCr.size());
		//MuCr = std::min(std::max(MuCr, .01), 1.);
	//}

	//if (!SF.empty()){
		//MuF = (1.-c) * MuF + c * lehmerMean(SF);
		//MuF = std::min(std::max(MuF, .01), 1.2);
	//}
//}

//void JADEManager::nextParameters(std::vector<double>& Crs, std::vector<double>& Fs){
	//int third = popSize/3;
	//std::vector<double> indices(popSize);
	//std::iota(indices.begin(), indices.end(), 0);
	//rng.shuffle(indices.begin(), indices.end());

	//// Update mutation rate
	//for (int i = 0; i < third; i++)
		//Fs[indices[i]] = rng.randDouble(0.,1.2);

	//for (int i = third; i < popSize; i++){
		//do {
			//Fs[indices[i]] = std::min(rng.normalDistribution(MuF, .1),1.2);	
		//} while ( Fs[indices[i]] <= 0.);
	//}

	//// Update crossover rate
	//for (int i = 0; i < popSize; i++)
		//Crs[i] = std::min(std::max(rng.normalDistribution(MuCr, .1),.0),1.);

	//previousFs = Fs;
	//previousCrs = Crs;
//}

//double JADEManager::lehmerMean(std::vector<double>const& SF) const {
	//double sumOfSquares = std::inner_product(SF.begin(), SF.end(), SF.begin(), 0.);
	//double sum = std::accumulate(SF.begin(), SF.end(), 0.);
	//return sumOfSquares/sum;
//}

// SHADE
SHADEManager::SHADEManager(int const popSize, int const K) : 
	ParameterAdaptationManager(popSize, K), H(popSize), MCr(K, std::vector<double>(H)), MF(K, std::vector<double>(H)), 
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

void SHADEManager::update(std::vector<std::vector<double>>const& targets, std::vector<std::vector<double>>const& trials){
	for (int i = 0; i < K; i++){
		std::vector<double> SF, SCr, delta;
		for (unsigned int j = 0; j < targets[i].size(); j++){
			if (trials[i][j] < targets[i][j]){
				SF.push_back(previousFs[i][j]); 
				SCr.push_back(previousCrs[i][j]);
				delta.push_back(std::abs(trials[i][j] - targets[i][j]));
			}
		}

		if (!SF.empty() && !SCr.empty()){
			std::vector<double> const _w = w(delta);
			MF[i][k[i]] = weightedLehmerMean(SF, _w);
			MCr[i][k[i]] = weightedMean(SCr, _w);
			k[i] = (k[i]+1)%H;
		}
	}
}

void SHADEManager::nextParameters(std::vector<double>& Fs, std::vector<double>& Crs, std::vector<int> const assignment){
	for (int i = 0; i < popSize; i++){
		int const randIndex = rng.randInt(0, H-1);

		// Update mutation rate
		double const MFr = MF[assignment[i]][randIndex];
		do{
			Fs[i] = std::min(rng.cauchyDistribution(MFr, .1), 1.);
		} while (Fs[i] <= 0.);

		// Update crossover rate
		double const MCrr = MCr[assignment[i]][randIndex];
		Crs[i] = std::min(std::max(rng.normalDistribution(MCrr, .1),0.),1.);
	}
 
	previousFs = Fs;
	previousCrs = Crs;
}

//NO ADAPTATION
ConstantParameterManager::ConstantParameterManager(int const popSize)
 : ParameterAdaptationManager(popSize), F(.5), Cr(.9){}

void ConstantParameterManager::update(std::vector<double>const& /*orig*/, std::vector<double>const& /*trials*/){
	//ignore
}

void ConstantParameterManager::nextParameters(std::vector<double>& Fs, std::vector<double>& Crs){
	// Update mutation rate
	std::fill(Fs.begin(), Fs.end(), F);
	// Update crossover rate
	std::fill(Crs.begin(), Crs.end(), Cr);
}

#pragma once
#include "solution.h"
#include <vector>
#include <set>
#include <map>
#include <functional>

class ParameterAdaptationManager {
protected:
	int const popSize;
	int const K;
public:
	ParameterAdaptationManager(int const popSize, int const K); 
	virtual ~ParameterAdaptationManager(){};
	virtual void nextParameters(std::vector<double>& Crs, std::vector<double>& Fs, std::vector<int>const& assignment)=0; 
	virtual void update(std::vector<std::vector<double>>const& orig, std::vector<std::vector<double>>const& trials)=0;
};

extern std::map<std::string, std::function<ParameterAdaptationManager*(int const, int const)>> const parameterAdaptations;

class SHADEManager : public ParameterAdaptationManager {
	private:
		int const H;
		
		std::vector<std::vector<double>> previousFs;
		std::vector<std::vector<double>> previousCrs;
		std::vector<std::vector<double>> MCr;
		std::vector<std::vector<double>> MF;
		std::vector<int> k; 

		double weightedLehmerMean(std::vector<double>const& x, std::vector<double>const& w) const;
		double weightedMean(std::vector<double>const& x, std::vector<double>const& w) const;
		std::vector<double> w(std::vector<double>const& delta) const;
	public:
		SHADEManager(int const popSize, int const K);
		void nextParameters(std::vector<double>& Crs, std::vector<double>& Fs, std::vector<int>const& assignment); 
		void update(std::vector<std::vector<double>>const& orig, std::vector<std::vector<double>>const& trials);
};

class ConstantParameterManager : public ParameterAdaptationManager {
	private:
		double const F;
		double const Cr;
	public:
		ConstantParameterManager(int const popSize, int const K);
		void nextParameters(std::vector<double>& Crs, std::vector<double>& Fs, std::vector<int>const& assignment); 
		void update(std::vector<std::vector<double>>const& orig, std::vector<std::vector<double>>const& trials);
};

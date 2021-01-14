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
	ParameterAdaptationManager(std::vector<Solution*>const& population, int const K); 
	virtual ~ParameterAdaptationManager(){};
	virtual void nextParameters(std::vector<double>& Fs, std::vector<double>& Crs, std::vector<int>const& assignment)=0; 
	virtual void update(std::vector<double>const& trialF)=0;
};

extern std::map<std::string, std::function<ParameterAdaptationManager*(std::vector<Solution*>const&, int const)>> 
	const parameterAdaptations;

class SHADEManager : public ParameterAdaptationManager {
	private:
		int const H;
		std::vector<double> previousFitness; 
		std::vector<double> previousFs;
		std::vector<double> previousCrs;
		std::vector<int> previousAssignment;
		std::vector<std::vector<double>> MCr;
		std::vector<std::vector<double>> MF;
		std::vector<int> k; 

		double weightedLehmerMean(std::vector<double>const& x, std::vector<double>const& w) const;
		double weightedMean(std::vector<double>const& x, std::vector<double>const& w) const;
		std::vector<double> w(std::vector<double>const& delta) const;
	public:
		SHADEManager(std::vector<Solution*>const& population, int const K);
		void nextParameters(std::vector<double>& Fs, std::vector<double>& Crs, std::vector<int>const& assignment); 
		void update(std::vector<double>const& trialF);
};

class ConstantParameterManager : public ParameterAdaptationManager {
	private:
		double const F;
		double const Cr;
	public:
		ConstantParameterManager(std::vector<Solution*>const& population, int const K);
		void nextParameters(std::vector<double>& Fs, std::vector<double>& Crs, std::vector<int>const& assignment); 
		void update(std::vector<double>const& trialF);
};

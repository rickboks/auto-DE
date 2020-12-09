#pragma once
#include "solution.h"
#include <vector>
#include <set>
#include <map>
#include <functional>

class ParameterAdaptationManager {
protected:
	int const popSize;
public:
	ParameterAdaptationManager(int const popSize); 
	virtual ~ParameterAdaptationManager(){};
	virtual void nextParameters(std::vector<double>& Crs, std::vector<double>& Fs)=0;
	virtual void update(std::vector<double>const& targets, std::vector<double>const& trials)=0;
};

extern std::map<std::string, std::function<ParameterAdaptationManager*(int const)>> const deAdaptations;

class JADEManager : public ParameterAdaptationManager{
private:
	std::vector<double> previousFs;
	std::vector<double> previousCrs;

	double MuCr;
	double MuF;
	double const c;
	double lehmerMean(std::vector<double>const& SF) const;
public:
	JADEManager(int const popSize);
	void nextParameters(std::vector<double>& Crs, std::vector<double>& Fs);
	void update(std::vector<double>const& orig, std::vector<double>const& trials);
};

class SHADEManager : public ParameterAdaptationManager {
	private:
		std::vector<double> previousFs;
		std::vector<double> previousCrs;

		int const H;
		std::vector<double> MCr;
		std::vector<double> MF;
		std::vector<int> r;

		int k;
		double weightedLehmerMean(std::vector<double>const& x, std::vector<double>const& w) const;
		double weightedMean(std::vector<double>const& x, std::vector<double>const& w) const;
		std::vector<double> w(std::vector<double>const& delta) const;
	public:
		SHADEManager(int const popSize);
		void nextParameters(std::vector<double>& Crs, std::vector<double>& Fs);
		void update(std::vector<double>const& orig, std::vector<double>const& trials);
};

class NoAdaptationManager : public ParameterAdaptationManager {
private:
	double const F;
	double const Cr;
public:
	NoAdaptationManager(int const popSize);
	void nextParameters(std::vector<double>& Crs, std::vector<double>& Fs);
	void update(std::vector<double>const& orig, std::vector<double>const& trials);
};

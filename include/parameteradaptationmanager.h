#pragma once
#include "solution.h"
#include "params.h"

using Eigen::ArrayXi;
using Eigen::MatrixXd;

class ParameterAdaptationManager {
protected:
	int const popSize;
	int const K;
public:
	static std::function<ParameterAdaptationManager*(int const, int const)> create(std::string const id);
	ParameterAdaptationManager(int const popSize, int const K); 
	virtual ~ParameterAdaptationManager(){};
	virtual void nextParameters(ArrayXd& Fs, ArrayXd& Crs, ArrayXi const& assignment)=0; 
	virtual void update(ArrayXd const& trialF)=0;
};

class SHADEManager : public ParameterAdaptationManager {
	private:
		int const H;
		ArrayXd previousFs;
		ArrayXd previousCrs;
		ArrayXi previousAssignment;
		MatrixXd MCr;
		MatrixXd MF;
		ArrayXi k; 

		double weightedLehmerMean(ArrayXd const& x, ArrayXd const& w) const;
		double weightedMean(ArrayXd const& x, ArrayXd const& w) const;
		ArrayXd w(ArrayXd const& delta) const;
	public:
		SHADEManager(int const popSize, int const K);
		void nextParameters(ArrayXd& Fs, ArrayXd& Crs, ArrayXi const& assignment); 
		void update(ArrayXd const& improvement);
};

class ConstantParameterManager : public ParameterAdaptationManager {
	private:
		double const F = params::F;
		double const Cr = params::Cr;
	public:
		ConstantParameterManager(int const popSize, int const K);
		void nextParameters(ArrayXd& Fs, ArrayXd& Crs, ArrayXi const& assignment); 
		void update(ArrayXd const& improvement);
};

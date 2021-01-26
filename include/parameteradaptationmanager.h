#pragma once
#include "solution.h"
#include <vector>
#include <set>
#include <map>
#include <functional>

using Eigen::VectorXi, Eigen::MatrixXd;

class ParameterAdaptationManager {
protected:
	int const popSize;
	int const K;
public:
	static std::function<ParameterAdaptationManager*(int const, int const)> create(std::string const id);
	ParameterAdaptationManager(int const popSize, int const K); 
	virtual ~ParameterAdaptationManager(){};
	virtual void nextParameters(VectorXd& Fs, VectorXd& Crs, VectorXi const& assignment)=0; 
	virtual void update(VectorXd const& trialF)=0;
};

class SHADEManager : public ParameterAdaptationManager {
	private:
		int const H;
		VectorXd previousFs;
		VectorXd previousCrs;
		VectorXi previousAssignment;
		MatrixXd MCr;
		MatrixXd MF;
		VectorXi k; 

		double weightedLehmerMean(VectorXd const& x, VectorXd const& w) const;
		double weightedMean(VectorXd const& x, VectorXd const& w) const;
		VectorXd w(VectorXd const& delta) const;
	public:
		SHADEManager(int const popSize, int const K);
		void nextParameters(VectorXd& Fs, VectorXd& Crs, VectorXi const& assignment); 
		void update(VectorXd const& improvement);
};

class ConstantParameterManager : public ParameterAdaptationManager {
	private:
		double const F = .5;
		double const Cr = .9;
	public:
		ConstantParameterManager(int const popSize, int const K);
		void nextParameters(VectorXd& Fs, VectorXd& Crs, VectorXi const& assignment); 
		void update(VectorXd const& improvement);
};

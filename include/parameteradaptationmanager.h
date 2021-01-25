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
	static std::function<ParameterAdaptationManager*(std::vector<Solution*>, int const)> create(std::string const id);
	ParameterAdaptationManager(std::vector<Solution*>const& population, int const K); 
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
		SHADEManager(std::vector<Solution*>const& population, int const K);
		void nextParameters(VectorXd& Fs, VectorXd& Crs, VectorXi const& assignment); 
		void update(VectorXd const& improvement);
};

class ConstantParameterManager : public ParameterAdaptationManager {
	private:
		double const F;
		double const Cr;
	public:
		ConstantParameterManager(std::vector<Solution*>const& population, int const K);
		void nextParameters(VectorXd& Fs, VectorXd& Crs, VectorXi const& assignment); 
		void update(VectorXd const& improvement);
};

#pragma once
#include <vector>
#include <string>
#include <functional>
#include "Eigen/Dense"

using Eigen::VectorXd;
class QualityManager {
	protected:
		int const K;
	public:
		static std::function<QualityManager* (int const)> create(std::string const id);
		QualityManager(int const K) : K(K){};
		virtual ~QualityManager(){};
		virtual void updateQuality(VectorXd &q, VectorXd const& r, 
				VectorXd const& p) const=0;
};

class WeightedSumQuality : public QualityManager {
	private:
		double const alpha = 0.8;
	public:
		WeightedSumQuality(int const K) : QualityManager(K){};
		void updateQuality(VectorXd &q, VectorXd const& r, 
				VectorXd const& p) const;
};

class RecPMQuality : public QualityManager {
	private:
		double const gamma = 0.1;
	public:
		RecPMQuality(int const K) : QualityManager(K){}
		void updateQuality(VectorXd &q, VectorXd const& r, 
				VectorXd const& p) const;
};

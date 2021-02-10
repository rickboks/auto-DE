#pragma once
#include <vector>
#include <string>
#include <functional>
#include "Eigen/Dense"

using Eigen::ArrayXd;
class QualityManager {
	protected:
		int const K;
	public:
		static std::function<QualityManager* (int const)> create(std::string const id);
		QualityManager(int const K) : K(K){};
		virtual ~QualityManager(){};
		virtual void updateQuality(ArrayXd &q, ArrayXd const& r, ArrayXd const& p) const=0;
};

class WeightedSumQuality : public QualityManager {
	private:
		double const alpha = 0.3;
	public:
		WeightedSumQuality(int const K) : QualityManager(K){};
		void updateQuality(ArrayXd &q, ArrayXd const& r, ArrayXd const& p) const;
};

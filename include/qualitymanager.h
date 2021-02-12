#pragma once
#include <vector>
#include <string>
#include <functional>
#include "Eigen/Dense"
#include "params.h"

using Eigen::ArrayXd;
class QualityManager {
	protected:
		int const K;
	public:
		static std::function<QualityManager* (int const)> create(std::string const id);
		QualityManager(int const K) : K(K){};
		virtual ~QualityManager(){};
		virtual void updateQuality(ArrayXd &q, ArrayXd const& r, std::vector<bool> const used) const=0;
};

class WeightedSumQuality : public QualityManager {
	private:
		double const alpha = params::WS_alpha;
	public:
		WeightedSumQuality(int const K) : QualityManager(K){};
		void updateQuality(ArrayXd &q, ArrayXd const& r, std::vector<bool> const used) const;
};

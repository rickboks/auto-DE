#pragma once
#include <vector>
#include <string>
#include "Eigen/Dense"
#include "coco.h"

using Eigen::ArrayXd;
class Solution {
	protected:
		ArrayXd x;
		bool evaluated;
		double fitness;
	public:
		Solution(int const D);
		virtual ~Solution();
		Solution(ArrayXd const x);
		int const D;
		void setX(int const dim, double const val);
		void setX(ArrayXd const& x);
		ArrayXd X() const;
		double X(int const dim) const;
		double evaluate (coco_problem_t* const problem);
		double getFitness() const;
		void setFitness(double const d);
		std::string positionString() const;
		void randomize(ArrayXd const& lowerBounds, ArrayXd const& upperBounds);
		bool operator < (Solution const& s) const;
		void copy (Solution const * const other);
};

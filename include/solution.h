#pragma once
#include <vector>
#include <string>
#include "Eigen/Dense"
#include "coco.h"

class Solution {
	protected:
		Eigen::VectorXd x;
		bool evaluated;
		double fitness;
	public:
		Solution(int const D);
		virtual ~Solution();
		Solution(Eigen::VectorXd const mutant);
		int const D;
		virtual void setX(Eigen::VectorXd const x, double const fitness);
		void setX(int const dim, double const val);
		void setX(Eigen::VectorXd x);
		Eigen::VectorXd getX() const;
		double getX(int const dim) const;
		double evaluate (coco_problem_t* const problem);
		double getFitness() const;
		void setFitness(double const d);
		std::string positionString() const;
		void randomize(Eigen::VectorXd const lowerBounds, Eigen::VectorXd const upperBounds);
		bool operator < (Solution const& s) const;
		void copy (Solution const * const other);
};

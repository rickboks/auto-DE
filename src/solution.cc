#include "solution.h"
#include "rng.h"
#include <iostream>

Solution::Solution(int const D) : x(D),  evaluated(false), fitness(std::numeric_limits<double>::max()), D(D){}
Solution::Solution(ArrayXd const x): x(x), evaluated(false), fitness(std::numeric_limits<double>::max()), D(x.size()){}
Solution::~Solution(){};

void Solution::setX(ArrayXd const& x){
	this->x = x;
	evaluated=false;
}

double Solution::getFitness() const{
	return fitness;
}

void Solution::setFitness(double const f){
	this->fitness = f;
	evaluated=true;
}

double Solution::evaluate(coco_problem_t* const problem) {
	if (!evaluated){
		evaluated = true;		
		coco_evaluate_function(problem, x.data(), &fitness);
	} 
	return fitness;
}

ArrayXd Solution::X() const {
	return x;
}

void Solution::randomize(ArrayXd const& lowerBounds, ArrayXd const& upperBounds){
	x = ArrayXd::NullaryExpr(D, [lowerBounds, upperBounds](Eigen::Index const i){
			return rng.randDouble(lowerBounds(i), upperBounds(i));}
		);
	evaluated = false;
}

bool Solution::operator < (const Solution& s) const {
	return fitness < s.getFitness();
}

void Solution::setX(int const dim, double const val){
	x(dim) = val;
	evaluated = false;
}

double Solution::X(int const dim) const {
	return x(dim);
}

void Solution::copy(Solution const* const other){
	x = other->x;
	evaluated = other->evaluated;
	fitness = other->fitness;
}

std::string Solution::positionString() const {
	std::string pos = "";
	for (int i = 0; i < D -1; i++){
		pos += std::to_string(x(i));
		pos += " ";
	}
	pos += std::to_string(x(D-1));
	return pos;
}

#pragma once
#include <vector>
#include <string>
#include "constrainthandler.h"

class MutationManager;
class CrossoverManager;

struct ConfigurationSpace {
	ConfigurationSpace(std::vector<std::string> const mutation, std::vector<std::string> const crossover, 
			int const D, ConstraintHandler* const ch);

	virtual ~ConfigurationSpace(){};

	std::vector<MutationManager*> mutation;
	std::vector<CrossoverManager*> crossover;
};

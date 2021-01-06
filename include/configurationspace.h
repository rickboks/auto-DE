#pragma once
#include <vector>
#include <string>

class MutationManager;
class CrossoverManager;
class ConstraintHandler;

struct ConfigurationSpace {
	ConfigurationSpace(std::vector<std::string> const mutation, std::vector<std::string> const crossover,
			ConstraintHandler* const ch);

	virtual ~ConfigurationSpace();
	std::vector<std::tuple<MutationManager*, CrossoverManager*>> getConfigurations() const; // return all combinations
	std::vector<MutationManager*> mutation;
	std::vector<CrossoverManager*> crossover;
};

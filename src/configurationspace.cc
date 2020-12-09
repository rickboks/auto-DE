#include "configurationspace.h"
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "parameteradaptationmanager.h"

ConfigurationSpace::ConfigurationSpace(std::vector<std::string> const mutation, std::vector<std::string> const crossover, 
		int const D, ConstraintHandler* const pam) {

	for (std::string m : mutation)
		this->mutation.push_back(mutations.at(m)(D, pam));

	for (std::string c : crossover)
		this->crossover.push_back(crossovers.at(c)(D));
}



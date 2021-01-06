#include "configurationspace.h"
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "parameteradaptationmanager.h"

ConfigurationSpace::ConfigurationSpace(std::vector<std::string> const mutation, std::vector<std::string> const crossover, 
		ConstraintHandler* const ch) {
	for (std::string m : mutation)
		this->mutation.push_back(mutations.at(m)(ch));
	for (std::string c : crossover)
		this->crossover.push_back(crossovers.at(c)());
}

ConfigurationSpace::~ConfigurationSpace(){
	for (MutationManager* m : mutation)
		delete m;
	for (CrossoverManager* c : crossover)
		delete c;
}

std::vector<std::tuple<MutationManager*, CrossoverManager*>> ConfigurationSpace::getConfigurations() const{
	std::vector<std::tuple<MutationManager*, CrossoverManager*>> configs;
	for (MutationManager* m : mutation)
		for (CrossoverManager* c : crossover)
			configs.push_back({m,c});
	return configs;
};

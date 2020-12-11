#pragma once
#include "mutationmanager.h"
#include "crossovermanager.h"
#include "parameteradaptationmanager.h"
#include "configurationspace.h"
#include "coco.h"

template <typename T>
class IOHprofiler_problem;

class IOHprofiler_csv_logger;

struct DEConfig {
	DEConfig(std::vector<std::string> mutation, std::vector<std::string> crossover, 
			std::string const adaptation, std::string const constraintHandler)
			: mutation(mutation), crossover(crossover), adaptation(adaptation), constraintHandler(constraintHandler){}

	std::vector<std::string> const mutation, crossover;
	std::string const adaptation, constraintHandler;
};

class DifferentialEvolution {
	private:
		DEConfig const config;
	public:
		DifferentialEvolution(DEConfig const config);
		void run(coco_problem_t* problem, int const evalBudget, int const popSize) const;
		std::string getIdString() const;
};

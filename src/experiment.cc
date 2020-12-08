#include <IOHprofiler_experimenter.h>
#include <random>
#include <set>
#include "desuite.h"
#include "differentialevolution.h"
#include "util.h"

DifferentialEvolution* de;

void algorithm
(std::shared_ptr<IOHprofiler_problem<double>> problem,
 std::shared_ptr<IOHprofiler_csv_logger> logger) {
    int const D = problem->IOHprofiler_get_number_of_variables(); 
    de->run(problem, logger, D*10000, 5 * D); 
}

void _run_experiment(bool const log) {
    de = new DifferentialEvolution(DEConfig("P1", "B", "S", "PM"));
	std::string templateFile = "./configuration.ini";
    std::string configFile = generateConfig(templateFile, de->getIdString());
    IOHprofiler_experimenter<double> experimenter(configFile,algorithm); 

    experimenter._set_independent_runs(5);
    experimenter._run();
    delete de;
}

int main(){
    bool const log = false;
    _run_experiment(log);
}

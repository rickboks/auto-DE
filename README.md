# auto-DE
A framework for Differential Evolution with adaptive selection of operators and parameters.

| Flags | Meaning | Examples | 
|-------|---------|---------|
| -d, --dimensions | comma-separated list of dimensionalities of the problems | -d 2,3,5|
| -f, --functions | comma-separated list or range notation of functions to include | -f 1-5 **or** -f 4,14,21|
| -i, --instances | comma-separated list or range notation of instances to include | -i 1-5 **or** -i 1,3,5 |
| -m, --mutation | comma-separated list of mutation options to use | -m BE1,RA1,TB2 |
| -c, --crossover | comma-separated list of crossover options to use | -c B,E |
| -F | Mutation rate, used when parameter adaptation is disabled | -F 0.9 |
| -C, --Cr | Crossover rate, used when parameter adaptation is disabled | -C 0.5 |
| --strategy | Strategy/operator adaptation method | --strategy A | 
| --parameter | Parameter adaptation method | --parameter S |
| --credit | Configuration credit assignment method | --credit CO |
| --reward | Configuration reward assignment method | --reward ER |
| --quality | Configuration quality update method | --quality WS |
| --probability | Configuration selection probability update method | --probability AP |
| --constraint | Boundary constraint handling method | --constraint RS |
| --id | String identifier of the algorithm (used for logging) | --id DE |
| --alpha | Alpha parameter used by the Weighted Sum (WS) quality method | --alpha 0.4 |
| --beta | Beta parameter used by the Adaptive Pursuit (AP) probability method | --beta 0.6 |
| --gamma | Gamma parameter used by Adaptive Pursuit and Probability Matching (controls p_min) | --gamma 3 |
| --popsize-multiplier | Number by which to multiply the dimensionality of the problem to obtain the population size | --popsize-multiplier 5 |
| --budget-multiplier | Number by which to multiply the dimensionality of the problem to obtain the evaluation budget | --budget-multiplier 10000 |
| --independent-runs | Number of times each problem instance should be repeated | --independent-runs 5 |
| --log-activations | Activate operator activation logging. Optional argument controls the interval in terms of iterations | --log-activations **or** --log-activations 10 |
| --log-parameters | Activate parameter logging. Optional argument controls the interval in terms of iterations | --log-parameters **or** --log-parameters 10 |
| --log-positions | Activate logging of solution positions. Optional argument controls the interval in terms of iterations | --log-positions **or** --log-positions 10 |
| --log-diversity | Activate population diversity logging. Optional argument controls the interval in terms of iterations | --log-diversity **or** --log-diversity 10 |
| --log-repairs | Activate logging of percentages of repaired solutions. Optional argument controls the interval in terms of iterations | --log-repairs **or** --log-repairs 10 |

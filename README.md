# auto-DE
A framework for Differential Evolution with adaptive selection of operators and parameters. The accompanying paper, with documentation about all the different options, will be provided soon. The generated algorithm(s) can be benchmarked on BBOB. Many options can be configured using command-line parameters, listed below. To compile, simply run:

```
make
```

## Parameters

| Flags | Meaning | Examples | 
|-------|---------|---------|
| -d, --dimensions | Comma-separated list of dimensionalities of the problems | -d 2,3,5|
| -f, --functions | Comma-separated list or range notation of functions to include | -f 1-5 **or** -f 4,14,21|
| -i, --instances | Comma-separated list or range notation of instances to include | -i 1-5 **or** -i 1,3,5 |
| -m, --mutation | Comma-separated list of mutation options to use | -m BE1,RA1,TB2 |
| -c, --crossover | Comma-separated list of crossover options to use | -c B,E |
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

## Operator adaptation strategy options
| Shorthand | Meaning |
|-----------|---------|
| A | Adaptive strategy |
| C | Constant strategy (no adaptation) | 
| R | Random strategy |

## Parameter adaptation options (F and Cr)
| Shorthand | Meaning |
|-----------|---------|
| S | SHADE parameter adaptation | 
| C | Constant parameters (no adaptation) |
## Mutation options
| Shorthand | Meaning |
|-----------|---------|
| RA1 | rand/1 |
| TB1 | target-to-best/1 |
| TB2 | target-to-best/2 |
| TR1 | target-to-rand/1 |
| TP1 | target-to-pbest/1 |
| BE1 | best/1 |
| BE2 | best/2 |
| RA2 | rand/2 |
| R2D | rand/2/dir |
| NSD | NSDE |
| TRI | trigonometric |
| TO1 | two-opt/1 |
| TO2 | two-opt/2 |
| PRX | proximity-based rand/1 |
| RAN | ranking-based target-to-pbest/1 |

## Crossover options
| Shorthand | Meaning |
|-----------|---------|
| B | Binomial |
| E | Exponential |

## Boundary Constraint Handling Method options
| Shorthand | Meaning |
|-----------|---------|
| DP | Death Penalty |
| RS | Resampling |
| RI | Reinitialization |
| PR |Projection |
| RF | Reflection |
| WR | Wrapping |
| TR | Boundary Transformation |
| RB | Rand Base |
| MB | Midpoint Base |
| MT | Midpoint Target |
| PM | Projection to Midpoint |
| PB | Projection to Base |

## Credit assignment options
| Shorthand | Meaning |
|-----------|---------|
| DR | Diversity ratio |
| SD | Squared diversity ratio |
| FD | Fitness improvement scaled by diversity ratio | 
| FS | Fitness improvement scaled by squared diversity ratio |
| FI | Fitness improvement |
| CO | Compass |
| PA | Pareto dominance |

## Reward assignment options
| Shorthand | Meaning |
|-----------|---------|
| AN | Average normalized reward |
| AA | Average absolute reward |
| EN | Extreme normalized reward |
| EA | Extreme absolute reward |
| ER | Extreme rank reward |
| AR | Average rank reward | 

## Quality assignment options
| Shorthand | Meaning |
|-----------|---------|
| WS | Weighted sum |

## Probability update methods
| Shorthand | Meaning |
|-----------|---------|
| AP | Adaptive Pursuit |
| PM | Probability Matching |


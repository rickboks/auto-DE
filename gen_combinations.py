#!/usr/bin/env python
import itertools

mutations = ["TB2", "RA1", "BE1", "TR1", "TO1"]
crossovers = ["B", "E"]
corrections = ["RS", "MT"]

mutation_combinations = []
for i in range(1,len(mutations)+1):
	mutation_combinations += [ '-m ' + ','.join(x) for x in itertools.combinations(mutations,i) ]

crossover_combinations = []
for i in range(1,len(crossovers)+1):
	crossover_combinations += [ '-c ' + ','.join(x) for x in itertools.combinations(crossovers,i) ]

correction_combinations = []
for i in range(1,len(corrections)+1):
	correction_combinations += [ '-b ' + ','.join(x) for x in itertools.combinations(corrections,i) ]

configs = list(itertools.product(mutation_combinations, crossover_combinations))
print(configs)
print(len(configs))

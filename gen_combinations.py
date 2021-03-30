#!/usr/bin/env python
import itertools

mutations = ["BE1", "RA1", "TP1", "TB2", "TR1", "TO1"]
crossovers = ["B", "E"]

def gen_params(m, c):
	return '-m ' + ','.join(m) + ' -c ' + ','.join(c)

mutation_combinations = []
for i in range(1,len(mutations)+1):
	mutation_combinations += [ list(x) for x in itertools.combinations(mutations,i) ]

crossover_combinations = []
for i in range(1,len(crossovers)+1):
	crossover_combinations += [ list(x) for x in itertools.combinations(crossovers,i) ]

configs = list(itertools.product(mutation_combinations, crossover_combinations))
configs = [c for c in configs if len(c[0]) * len(c[1]) > 1]

config_strings = [ gen_params(*x) for x in configs ]

outfile = open("parameter_combinations.dat", "w")
for s in config_strings:
	outfile.write("{}\n".format(s))
outfile.close()

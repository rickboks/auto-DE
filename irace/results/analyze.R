library(irace)
args = commandArgs(trailingOnly=TRUE)
for (arg in args){
	load(arg)
	last <- length(iraceResults$iterationElites)
	id <- iraceResults$iterationElites[last]
	x <- getConfigurationById(logFile = arg, ids = id)
	print(arg)
	print(x)
}

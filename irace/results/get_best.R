args = commandArgs(trailingOnly=TRUE)
for (val in args){
	obj <- get(load(val))$state$eliteConfigurations
	credit <- substr(val, 1, 2)

	if (credit %in% c("CO", "PA"))
		cat(paste(credit, " A:", obj$alpha[1], " B:", obj$beta[1], " M:", obj$pMin[1], sep=""))
	else
		cat(paste(credit, " A:", obj$alpha[1], " M:", obj$pMin[1], sep=""))
	cat("\n")
}

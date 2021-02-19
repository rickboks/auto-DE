#!/usr/bin/env fish
set d "$HOME/auto-DE/irace"

set cred $argv[1]
set prob (cat best_configs.dat | awk -v c=$cred '$2 == c {print $6}') 

if test ! (count $prob) = 1
	echo "Invalid credit manager"
	exit
end

mkdir -p results/$cred logs/$cred

irace --parameter-file "$d/parameters-$prob.txt" \
	--train-instances-file "$d/instances-list.txt" \
	--configurations-file "$d/configurations-$prob.txt" \
	--forbidden-file "$d/forbidden-$prob.txt" \
	--target-runner "$d/runners/$cred.runner" \
	--max-experiments "1" \
	--parallel (nproc --all) \
	-l results/$cred/result.Rdata \
	-s $d/scenario.txt

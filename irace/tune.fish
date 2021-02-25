#!/usr/bin/env fish
if test (count $argv) -lt 1
	echo "ERROR: no credit manager passed"
	exit
end

set d "$HOME/auto-DE/irace"
set cred $argv[1]

mkdir -p results/$cred logs/$cred

irace --parameter-file "$d/parameters.txt" \
	--train-instances-file "$d/instances-list.txt" \
	--forbidden-file "$d/forbidden.txt" \
	--target-runner "$d/runners/$cred.runner" \
	--max-experiments "500000" \
	--parallel (nproc --all) \
	--digits 2 \
	-l results/$cred/result.Rdata \
	-s $d/scenario.txt \
	--exec-dir $HOME/auto-DE/irace
	#--configurations-file "$d/configurations.txt" \

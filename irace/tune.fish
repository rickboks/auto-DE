#!/usr/bin/env fish
if test (count $argv) -lt 1
	echo "ERROR: no credit manager passed"
	exit
end

set d "$HOME/auto-DE/irace"
set cred $argv[1]
set HOURS 62
set threads (nproc --all)

mkdir -p results logs/$cred

irace --parameter-file "$d/parameters.txt" \
	--train-instances-file "$d/instances-list.txt" \
	--forbidden-file "$d/forbidden.txt" \
	--target-runner "$d/runners/$cred.runner" \
	--seed 44 \
	--max-time (math "$threads * $HOURS * 3600") \
	--parallel $threads \
	--digits 2 \
	-l results/$cred.Rdata \
	-s $d/scenario.txt \
	--exec-dir $HOME/auto-DE/irace \
	--budget-estimation (math "0.02 * (6 / $threads)") \

#!/usr/bin/env fish
if test (count $argv) -lt 1
	echo "ERROR: no credit manager passed"
	exit
end

set d "$HOME/auto-DE/irace"
set cred $argv[1]
set HOURS 60

mkdir -p results logs/$cred

irace --parameter-file "$d/parameters.txt" \
	--train-instances-file "$d/instances-list.txt" \
	--forbidden-file "$d/forbidden.txt" \
	--target-runner "$d/runners/$cred.runner" \
	--max-time (math "$HOURS * 3600") \
	--parallel (nproc --all) \
	--digits 2 \
	-l results/$cred.Rdata \
	-s $d/scenario.txt \
	--exec-dir $HOME/auto-DE/irace

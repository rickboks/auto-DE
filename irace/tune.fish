#!/usr/bin/env fish
set d "$HOME/auto-DE/irace"
set prob "AP"

irace --parameter-file "$d/parameters-$prob.txt" \
--train-instances-file "$d/instances-list.txt" \
--configurations-file "$d/configurations-$prob.txt" \
--forbidden-file "$d/forbidden-$prob.txt" \
--target-runner "$d/target-runner" \
--max-experiments "10000" \
--parallel (nproc --all) \
-s $d/scenario.txt



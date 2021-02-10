#!/usr/bin/env fish
set executable "$HOME/auto-DE/experiment"

set credit "DR" "SD" "FD" "FS" "FI" "CO"
set reward "AA" "AN" "EA" "EN"
set quality "WS"
set probability "PM" "AP"
set threads "32"

set hosts (preserve -llist | grep "$USER" | cut -f9- | tr ' ' ',')
set configs (printf '%s\n' $credit" "$reward" "$quality" "$probability |\
	awk -v exe=$executable '{printf("%1$s -C %2$s -r %3$s -q %4$s -p %5$s -I %2$s_%3$s_%4$s_%5$s\n",exe,$1,$2,$3,$4)}')

set num_configs (count $configs)	
set num_nodes (count (string split ',' $hosts))
set nodes_required (math (count $configs) / $threads)

if test $num_nodes -eq 0
	printf "Error: no nodes reserved\n"
	exit
end

printf "Running %d configurations on %d nodes.\n" $num_configs $num_nodes
if test $num_nodes -lt $nodes_required
	printf "WARNING: fewer nodes reserved than required to run all configurations at once\n"
end

if test (read -n1 -P "OK? [y/n]") = "y"
	parallel --sshdelay 0.1 --workdir (dirname "$executable") -S "$hosts" -u -j $threads --colsep ' ' ::: $configs
	printf "Done!\n"
end

#!/usr/bin/env fish
set executable "$HOME/auto-DE/experiment"

set credit "DR" "SD" "FD" "FS" "FI" "CO"
set reward "AA" "AN" "EA" "EN"
set quality "WS"
set probability "PM" "AP"
set threads "32"

set hosts (preserve -llist | grep "$USER" | cut -f9- | tr ' ' ',')
set configs (printf '%s\n' $credit" "$reward" "$quality" "$probability |\
	awk '{printf("%s %s_%s_%s_%s\n",$0,$1,$2,$3,$4);}')

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
	parallel --sshdelay 0.1 --workdir (dirname "$executable") -S "$hosts" -u -j $threads --colsep ' '\
		$executable -C {1} -r {2} -q {3} -p {4} -I {5} ::: $configs
	printf "Done!\n"
end

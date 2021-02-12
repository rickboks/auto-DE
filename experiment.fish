#!/usr/bin/env fish
set executable "$HOME/auto-DE/experiment"

set credit "DR" "SD" "FD" "FS" "FI" "CO" "PA"
set reward "AA" "AN" "EA" "EN"
set quality "WS"
set probability "PM" "AP"
set mutation "BE1,RA1,TB2"
set crossover "B,E"
set threads "32"
set data_folders "extra_data" "exdata"

set hosts (preserve -llist | grep "$USER" | cut -f9- | tr ' ' ',')
set configs (printf '%s\n' $credit" "$reward" "$quality" "$probability" "$mutation" "$crossover |\
	awk -v exe=$executable\
	'{printf("%1$s -C %2$s -r %3$s -q %4$s -p %5$s -m %6$s -c %7$s -I %2$s_%3$s_%5$s\n",exe,$1,$2,$3,$4,$5,$6)}') \
	"$executable -s R -m "$mutation" -c "$crossover" -I RANDOM" \
	"$executable -s C -m RA1 -c E -I DE_rand_1_exp" \
	"$executable -s C -m RA1 -c B -I DE_rand_1_bin" \
	"$executable -s C -m BE1 -c E -I DE_best_1_exp" \
	"$executable -s C -m BE1 -c B -I DE_best_1_bin" \
	"$executable -s C -m TB2 -c E -I DE_ttb_2_exp"  \
	"$executable -s C -m TB2 -c B -I DE_ttb_2_bin"  \

set num_configs (count $configs)
set num_nodes (count (string split ',' $hosts))
set nodes_required (math (count $configs) / $threads)

if test $num_nodes -eq 0
	printf "Error: no nodes reserved\n"
	exit
end

for folder in $data_folders
	if test (ls $folder | count) -gt 0
		if test (read -n1 -P "Folder \"$folder\" is not empty. Should I remove everything in it? [y\n]" ) = "y"
			rm -rf $folder/*
		end
	end
end

printf "Running %d configurations on %d nodes.\n" $num_configs $num_nodes
if test $num_nodes -lt $nodes_required
	printf "WARNING: fewer nodes reserved than required to run all configurations at once\n"
end

if test (read -n1 -P "OK? [y/n]") = "y"
	parallel --sshdelay 0.1 --workdir (dirname "$executable") -S "$hosts" -u -j $threads --colsep ' ' ::: $configs
	printf "Done!\n"
end

#!/usr/bin/env fish
set executable "$HOME/auto-DE/experiment"

set mutation "BE1" "RA1" "RA2" "TB1" 
set crossover "B" "E"
set correction "PR" "RS" "DP" "MT"
set threads "32"

set data_folders "extra_data" "exdata"

set hosts (preserve -llist | grep "$USER" | cut -f9- | tr ' ' ',')
set configs (printf '%s\n' $mutation" "$crossover" "$correction |\
	awk -v exe=$executable\
	'{printf("%1$s -s C -m %2$s -c %3$s -b %4$s -I %2$s_%3$s_%4$s\n",exe,$1,$2,$3)}')

set num_configs (count $configs)
set num_nodes (count (string split ',' $hosts))
set nodes_required (math (count $configs) / $threads)

mkdir -p /var/scratch/$USER/{exdata,extra_data}

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
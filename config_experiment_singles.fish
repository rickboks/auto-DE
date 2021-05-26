#!/usr/bin/env fish
set executable "$HOME/auto-DE/experiment"
set threads "32"
set data_folders "extra_data" "exdata"

set base_params (cat irace/tuned_configs.dat | grep "^--credit CO")
set hosts (preserve -llist | grep "$USER" | cut -f9- | tr ' ' ',')
set configs "$executable $base_params "(cat parameter_combinations_singles.dat)

printf '%s\n' $configs

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

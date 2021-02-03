#!/usr/bin/env fish
set executable "$HOME/auto-DE/experiment"

set credit "DR" "SD" "FD" "FS" "FI" "CO"
set probability "PM" "AP"
set mutation "RA1,BE1,TB2"
set crossover "B,E" "B"
set threads "32"

set hosts (preserve -llist | grep "$USER" | cut -f9- | tr ' ' ',')
set configs (printf '%s\n' $mutation" "$crossover" "$credit" "$probability | awk '{printf("%s %s_%s_%s\n",$0,$2,$3,$4);}')

printf "Running %d configurations on %d nodes ... " (count $configs) (count (string split ',' $hosts))

parallel --sshdelay 0.1 --workdir (dirname "$executable") -S "$hosts" -u -j $threads --colsep ' '\
	$executable -m {1} -c {2} -C {3} -p {4} -I {5} ::: $configs

printf "done!\n"


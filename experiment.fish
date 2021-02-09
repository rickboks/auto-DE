#!/usr/bin/env fish
set executable "$HOME/auto-DE/experiment"

set credit "DR" "SD" "FD" "FS" "FI" "CO"
set reward "AA" "AN" "EA" "EN"
set quality "WS" "BM"
set probability "PM" "AP"
set threads "32"

set hosts (preserve -llist | grep "$USER" | cut -f9- | tr ' ' ',')
set configs (printf '%s\n' $credit" "$reward" "$quality" "$probability |\
	awk '{printf("%s %s_%s_%s_%s\n",$0,$1,$2,$3,$4);}')

printf "Running %d configurations on %d nodes ... " (count $configs) (count (string split ',' $hosts))
exit

parallel --sshdelay 0.1 --workdir (dirname "$executable") -S "$hosts" -u -j $threads --colsep ' '\
	$executable -C {1} -r {2} -q {3} -p {4} -I {5} ::: $configs

printf "done!\n"

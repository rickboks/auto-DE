#!/usr/bin/env fish
set executable "$HOME/auto-DE/experiment"

set credit "DR" "SD" "FI"
set mutation "BE2,RA2,TB1,TP1"
set crossover "B"
set dimensions "20"
set instances "1-24"
set threads "32"

set hosts (preserve -llist | grep "$USER" | cut -f9- | tr ' ' ',')
set configs (printf '%s\n' $mutation" "$crossover" "$credit | awk '{printf("%s %s_%s_%s\n",$0,$1,$2,$3);}')
parallel --workdir (dirname "$executable") -u -S $hosts -j $threads --colsep ' '\
	$executable -m {1} -c {2} -C {3} -I {4} ::: $configs

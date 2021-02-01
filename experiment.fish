#!/home/rmboks/.local/bin/fish
set hostfile "hostfile"
set credit "DR" "SD" "FI"
set mutation "BE2,RA2,TB1,TP1"
set crossover "B"
set dimensions "20"
set instances "1-24"
set threads "32"

set executable "/home/rmboks/auto-DE/experiment"

preserve -llist | grep rmboks | cut -f9- | tr ' ' '\n' > $hostfile

set configs (printf '%s\n' $mutation" "$crossover" "$credit | awk '{printf("%s %s_%s_%s\n",$0,$1,$2,$3);}')

printf '%s\n' $configs

printf '%s\n' $configs | parallel -u --sshloginfile $hostfile -j $threads --colsep ' ' $executable -m {1} -c {2} -C {3} -I {4}

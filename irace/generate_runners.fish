#!/usr/bin/env fish
for c in (cat best_configs.dat)
	set credit (string split -- " " $c)[2]
	awk -v cred="$credit" \
		'/^CREDIT=/{sub(/__CREDIT__/,cred)} {print}' \
		target-runner-template > runners/$credit.runner
end

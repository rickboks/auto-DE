#!/usr/bin/env fish
for c in (cat best_configs.dat)
	set credit (string split -- " " $c)[2]
	awk -v conf="$c" -v cred="$credit" \
		'/^FIXED_PARAMS=/{sub(/__FIXED_PARAMS__/,conf)};
		/^CREDIT=|^LOG_DIR=/{sub(/__CREDIT__/,cred)} {print}' \
		target-runner-template > runners/$credit.runner
end

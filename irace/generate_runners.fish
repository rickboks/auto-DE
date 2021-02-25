#!/usr/bin/env fish
for c in "FI" "FD" "FS" "DR" "SD" "PA" "CO"
	awk -v cred="$c" \
		'/^CREDIT=/{sub(/__CREDIT__/,cred)} {print}' \
		target-runner-template > runners/$c.runner
end

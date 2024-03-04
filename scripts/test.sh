#! /bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
WC2=$SCRIPT_DIR/../bin/wc2

print_wc2_results() {
	if [ "$1" = "$2" ]; then RESULT="PASS"; else RESULT="FAIL"; fi
	printf -- "-------------------\n"
	printf "wc:\n$1\n\n"
	printf "wc2:\n$2\n\n"
	printf "Result: $RESULT\n"
	printf -- "-------------------\n"
}

if [ -e $WC2 ] && [ -e $1 ]; then 
	printf "\nSingle file input\n"
	print_wc2_results "$(wc $1)" "$($WC2 $1)"

	printf "\nMultiple file input\n"
	print_wc2_results "$(wc $1 $1)" "$($WC2 $1 $1)"

	printf "\nRead from stdin\n"
	print_wc2_results "$(cat $1 | wc)" "$(cat $1 | $WC2)"
else
	printf "Test file $1 not found\n"
fi

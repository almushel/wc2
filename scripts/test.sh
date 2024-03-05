#! /bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BIN_DIR=$SCRIPT_DIR/../bin
WC2=$BIN_DIR/wc2

print_wc2_results() {
	if [ "$1" = "$2" ]; then RESULT="PASS"; else RESULT="FAIL"; fi
	printf -- "-------------------\n"
	printf "wc:\n$1\n\n"
	printf "wc2:\n$2\n\n"
	printf "Result: $RESULT\n"
	printf -- "-------------------\n"
}

# NOTE: Tests currently fail for >2 inputs for both multiple file and --files0-from
# Difference between outputs is not visible to the eye, so it's probably some whitespace somewhere

if [ -e $WC2 ] && [ -e $1 ]; then 
	printf "\nSingle file input\n"
	print_wc2_results "$(wc $1)" "$($WC2 $1)"

	printf "\nMultiple file input\n"
	print_wc2_results "$(wc $1 $1)" "$($WC2 $1 $1)"

	printf "\nRead from stdin\n"
	print_wc2_results "$(cat $1 | wc)" "$(cat $1 | $WC2)"

	printf "$1\0$1" > $BIN_DIR/files.txt
	printf "\nRead file list from file\n"
	print_wc2_results "$(wc --files0-from=$BIN_DIR/files.txt)" "$($WC2 --files0-from=$BIN_DIR/files.txt)"
else
	printf "Test file $1 not found\n"
fi

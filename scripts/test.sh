#! /bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BIN_DIR=$SCRIPT_DIR/../bin
WC2=$BIN_DIR/wc2
OPTIONS=-cmlwL

print_wc2_results() {
	printf -- "-------------------\n"
	if [ "$1" = "$2" ]
	then 
		printf "Result: PASS\n"
	else 
		printf "wc:\n$1\n\n"
		printf "wc2:\n$2\n\n"
		printf "Result: FAIL\n"
	fi
	printf -- "-------------------\n"
}

# NOTE: Tests currently fail for >2 inputs for both multiple file and --files0-from
# Difference between outputs is not visible to the eye, so it's probably some whitespace somewhere

if [ -e $WC2 ] && [ -e $1 ]; then 
	printf "\nSingle file input\n"
	print_wc2_results "$(wc $OPTIONS $1)" "$($WC2 $OPTIONS $1)"

	printf "\nMultiple file input\n"
	print_wc2_results "$(wc $OPTIONS $1 $1)" "$($WC2 $OPTIONS $1 $1)"

	printf "\nRead from stdin\n"
	print_wc2_results "$(cat $1 | wc $OPTIONS)" "$(cat $1 | $WC2 $OPTIONS)"

	printf "$1\0$1" > $BIN_DIR/files.txt
	printf "\nRead file list from file\n"
	print_wc2_results "$(wc $OPTIONS --files0-from=$BIN_DIR/files.txt)" "$($WC2 $OPTIONS --files0-from=$BIN_DIR/files.txt)"

	# NOTE: Apparently, wc doesn't worry about padding or column alignment for file lists passed through stdin???
	printf "\nRead file list from stdin\n"
	print_wc2_results "$(printf "$1\0$1" | wc $OPTIONS --files0-from=-)" "$(printf "$1\0$1" | $WC2 $OPTIONS --files0-from=-)"

else
	printf "Test file $1 not found\n"
fi

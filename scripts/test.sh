#! /bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BIN_DIR=$SCRIPT_DIR/../bin
WC2=$BIN_DIR/wc2
OPTIONS=-cmlwL

param_cmp() {
	IFS=' '
	read -ra first <<< $1
	read -ra second <<< $2

	for i in "${!first[@]}"; do
		if [ ${first[$i]} != ${second[$i]} ]; then
			return 1
		fi
	done

	return 0
			
}

str_cmp() {
	if [ "$1" = "$2" ]; then
		return 0
	fi
	return 1
}

print_wc2_results() {
	printf -- "-------------------\n"
	param_cmp "$1" "$2"; VR=$?
	str_cmp "$1" "$2"; FR=$?

	if [ $VR != 0 ] || [ $FR != 0 ]; then 
		printf "wc:\n$1\n\n"
		printf "wc2:\n$2\n\n"
	fi

	if [ $VR = 0 ]; then printf "Values: PASS | "; else printf "Values: FAIL | "; fi
	if [ $FR = 0 ]; then printf "Formatting: PASS\n"; else printf "Formatting: FAIL\n"; fi
	printf -- "-------------------\n"
}

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

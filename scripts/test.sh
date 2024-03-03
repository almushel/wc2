#! /bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BIN=$SCRIPT_DIR/../bin/wc2

if [ -e $BIN ] && [ -e $1 ]; then 
	printf "\nSingle file input\n"
	printf -- "-------------------\n"
	printf "wc:\n"  && wc $1
	printf "wc2:\n" && $BIN $1

	printf "\nMultiple file input\n"
	printf -- "-------------------\n"
	printf "wc:\n"  && wc $1 $1
	printf "wc2:\n" && $BIN $1 $1

	printf "\nRead from stdin\n"
	printf -- "-------------------\n"
	printf "wc:\n"  && cat $1 | wc
	printf "wc2:\n" && cat $1 | $BIN
else
	printf "Test file $1 not found\n"
fi

#! /bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SRC_DIR=$SCRIPT_DIR/../src
BIN_DIR=$SCRIPT_DIR/../bin

if [ -d $BIN ]; then 
	mkdir $BIN_DIR
fi

gcc $SRC_DIR/main.c -o $BIN_DIR/wc2

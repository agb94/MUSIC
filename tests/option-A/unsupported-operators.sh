#!/bin/sh
# When user provides option -A for an unsupported operator
# COMUT exits on error

if test $# = 0; then
    echo "Usage: sh filename.sh executable-COMUT"
    echo "Error: no executable-COMUT file was given"
    exit 1
fi

# DIR: the directory that this script is in
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Executing $0"
cd input-src

# Exit script if there is no input source file
if test `find . -type f -name \* | wc -l` = 0 ; then
    exit
fi

for TEST_INPUT in *
do
	cd $DIR

    # make folder name in output directory with input source name without .c
    OUTPUT_FOLDER_NAME=`echo "output/$TEST_INPUT" | sed 's/.\{2\}$//'`

    COUNT=0
    while read LINE
    do
    	COUNT=$((COUNT+1))
    	mkdir $OUTPUT_FOLDER_NAME

    	echo "$1 input-src/${TEST_INPUT} -o $OUTPUT_FOLDER_NAME -m $LINE -A something"
    	$1 input-src/${TEST_INPUT} -o $OUTPUT_FOLDER_NAME -m $LINE -A something > /dev/null 2>&1

    	# The test success if exit value is NOT 0
		# and no files are generated in output folder
		if test $? != 0 && test `find ${OUTPUT_FOLDER_NAME} -type f -name \* | wc -l` = 0
		then
		    echo "[SUCCESS ${COUNT}/23] $TEST_INPUT option -A not supported for $LINE"
		else
		    echo "[FAIL ${COUNT}/23] $TEST_INPUT option -A not supported for $LINE"
		fi

    	# Remove created output folder for this input source file
    	rm -R $OUTPUT_FOLDER_NAME
    done < unsupported-operators.txt

    cd input-src
done
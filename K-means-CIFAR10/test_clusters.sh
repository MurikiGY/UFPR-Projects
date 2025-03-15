#!/bin/bash

#DIR=${1:-./}
#CLUSTER_DIR=${2:-cluster}

DIR="results"
CLUSTER_DIR=$1

DIRECTORIES=$(find "$DIR" -regextype sed -regex "${DIR}/*${CLUSTER_DIR}_[0-9]*" -type d)

SCORE=0

while read -r C_DIR
do
    echo 
    echo "$C_DIR"

    FILES=$(find "$C_DIR" -maxdepth 1 -mindepth 1 -name "*png" -type f -printf "%f\n") 

    TOTAL=$(echo "$FILES" | wc -l)
    CLASS_COUNT=$(echo "$FILES" | cut -d'_' -f2 | cut -d'.' -f1 | sort | uniq -c | tr -s ' ' | cut -d' ' -f2-)

    echo "Found $TOTAL files"
    while read -r LINE
    do
        NUM=$(echo ${LINE} | cut -d' ' -f1)
        CLASS=$(echo ${LINE} | cut -d' ' -f2)
        PERCENT=$(echo "scale=4;($NUM/$TOTAL)" | bc -l)
        echo "$NUM $PERCENT% ${CLASS}"

        SCORE=$(echo "scale=4;$SCORE + ($NUM*$PERCENT)" | bc -l)
    done <<< "$CLASS_COUNT"

done <<< "$DIRECTORIES"

echo "Final score: $(echo "scale=4; $SCORE/1000" | bc -l)"
echo "Final score: $(echo "scale=4; $SCORE/10000" | bc -l)"


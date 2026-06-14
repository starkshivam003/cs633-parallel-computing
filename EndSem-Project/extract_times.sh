#!/bin/bash

echo "P,size,run,time" > timings.csv

for file in *.out
do
    # detect P and size from filename
    if [[ $file == *np32*n120* ]]; then P=32; size=120
    elif [[ $file == *np48*n120* ]]; then P=48; size=120
    elif [[ $file == *np64*n120* ]]; then P=64; size=120
    elif [[ $file == *np96*n120* ]]; then P=96; size=120
    elif [[ $file == *np32*n240* ]]; then P=32; size=240
    elif [[ $file == *np48*n240* ]]; then P=48; size=240
    elif [[ $file == *np64*n240* ]]; then P=64; size=240
    elif [[ $file == *np96*n240* ]]; then P=96; size=240
    else
        continue
    fi

    run=1

    # extract all lines that are ONLY numbers (time lines)
    grep -E '^[0-9]+\.[0-9]+$' "$file" | while read t
    do
        echo "$P,$size,$run,$t" >> timings.csv
        run=$((run+1))
    done

done

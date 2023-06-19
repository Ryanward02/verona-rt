#!/bin/bash

# Ensure you run from current directory.
cd /Users/ryanward/Documents/git_repos/verona-rt

# Recompiles any changes to the benchmark that have been made, then runs the benchmark tools that we have created -
# It still compiled the full test suite, but avoids running everything (which takes around 2 hours)

# Full compilation:

# rm -rf build_ninja
# mkdir build_ninja
# cd build_ninja
# cmake .. -GNinja -DCMAKE_BUILD_TYPE=Debug
# ninja

# After fully compiled once, only these need to run upon further builds:
cd build_ninja
ninja

# Test if the build failed, and terminate if so.
if [ $? -ne 0 ]
then
    echo build failed
    date
    exit -1
fi



for totalCowns in 100
do
    for cownPop in "zipfian[2]" 
    do
        for behaviourCowns in 15
        do
            for whenCount in 200
            do
                for servTime in "fixed[145]"
                do
                    ./test/bm-con-lotsofcowns --cownPop "$cownPop" --servTime "$servTime" --totalCowns "$totalCowns" --behaviourCowns "$behaviourCowns" --whenCount "$whenCount" --notParallel
                    ./test/bm-con-lotsofcowns --cownPop "$cownPop" --servTime "$servTime" --totalCowns "$totalCowns" --behaviourCowns "$behaviourCowns" --whenCount "$whenCount" --parallel

                done
            done
        done
    done
done

# USAGE: ./test/bm-con-lotsofcowns --cownPop uniform|zipfian[zipfConst] --servTime fixed[ms]|bimodal[fast|slow|pct]|exp[lambda] --totalCowns no_of_cowns --behaviourCowns PERCENTAGE_PER_BEHAVIOUR --whenCount no_of_behaviours --parallel|--notParallel
# ./test/bm-con-lotsofcowns --cownPop "$cownPop" --servTime "$servTime" --totalCowns "$totalCowns" --behaviourCowns "$behaviourCowns" --whenCount "$whenCount" --notParallel

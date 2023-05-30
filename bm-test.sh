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


# We can then call the benchmark from the build folder, since ninja has created executables for us.
./test/bm-con-lotsofcowns --cownPop zipfian[2] --servTime fixed[100] --totalCowns 500 --behaviourCowns 40 --whenCount 500 --parallel
# ./test/bm-con-testingimp

# Or we can run the test with ctest
# cd test
# ctest -R bm-con-lotsofcowns

# Notes:
# Spinning time
# Cown selection (popularity & no of cowns/behaviours)
# Zipfian distribution
# array of samples
#   per thread variables
#   thread local 

# Inter-arrival distribution
#  Poisson (exponential) gaps

# Verona Runtime
#  Check Scheduling code. Code behind when. What happens when when() is called.
#!/bin/bash

# Ensure you run from current directory.
cd /Users/ryanward/Documents/git_repos/verona-rt

# Recompiles any changes to the benchmark that have been made, then runs the benchmark tools that we have created -
# It still compiled the full test suite, but avoids running everything (which takes around 2 hours)

# rm -rf build_ninja
# mkdir build_ninja
cd build_ninja
# cmake .. -GNinja -DCMAKE_BUILD_TYPE=Debug
ninja

# We can then call each benchmark from the build folder, since ninja has created executables for us.
# if [ $? -ne 0 ]
# then
#     echo build failed
#     exit -1
# fi
# date


./test/bm-con-lotsofcowns
# ./test/bm-con-lotsofwhens
# ./test/bm-con-servicetime

# Or we can run the test with ctest
# cd test
# ctest -R bm-con-lotsofcowns
# ctest -R bm-con-lotsofwhens
# ctest -R bm-con-servicetime

# Notes:
# Spinning time
# Cown selection (popularity & no of cowns/behaviours)
# Zipfian distribution
# array of samples
#   per thread variables
#   thread local 
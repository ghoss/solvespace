#!/bin/sh
#
# Wrapper for cmake
#
# Guido Hoss, 3. April 2018 11:51:07 MESZ

rm -rf CMakeCache.txt CMakeFiles/
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
echo "Now run make -j8"
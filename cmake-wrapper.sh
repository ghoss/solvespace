#!/bin/sh
#
# Wrapper for cmake
#
# Guido Hoss, 3. April 2018 11:51:07 MESZ
#
# Will require 
# - libpng, best inatalled with Homebrew and "brew install libpng"
# - cairo, "brew install cairo"

rm -rf CMakeCache.txt CMakeFiles/ build
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
echo "Now run make -j8"

#!/bin/sh
mkdir -p build/Debug
cd build/Debug
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../..
ninja
cd ../..
mkdir -p build/Release
cd build/Release
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ../..
ninja

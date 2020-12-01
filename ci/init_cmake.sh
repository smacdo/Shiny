#!/bin/sh
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_CLANG_TIDY=ON .. 

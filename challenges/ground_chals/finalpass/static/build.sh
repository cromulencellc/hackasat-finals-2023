#!/bin/bash

cmake -DCMAKE_BUILD_TYPE=Debug -B build .
cmake --build build

if [[ -n "$1" ]]; then
    cp build/finalpass $1
fi

#!/bin/bash
cd ~/projects/cpp-file-scanner
cmake -S . -B build-shm-asan -DCMAKE_CXX_FLAGS="-fsanitize=address -g -O1" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address"
cmake --build build-shm-asan

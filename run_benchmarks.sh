#!/bin/bash

# Target directory for the benchmark
TARGET_DIR=${1:-/usr/lib}

# Build directory name
BUILD_DIR="build_benchmark"

function build_and_run() {
    local opt_level=$1
    local build_type=$2
    
    echo "======================================================"
    echo "Benchmarking with Optimization: $opt_level ($build_type)"
    echo "Target Directory: $TARGET_DIR"
    echo "======================================================"
    
    # Configure CMake
    cmake -S . -B ${BUILD_DIR}_${opt_level} \
          -DCMAKE_BUILD_TYPE=${build_type} \
          -DCMAKE_CXX_FLAGS="${opt_level}" > /dev/null 2>&1
    
    # Build benchmark_scanner
    cmake --build ${BUILD_DIR}_${opt_level} --target benchmark_scanner > /dev/null 2>&1
    
    echo "Workers | Files | Bytes | Discovery (us) | Processing (us) | Throughput (MB/s) | Files/sec | Avg Hash (us) | Avg Detect (us)"
    echo "--------------------------------------------------------------------------------------------------------------------------"
    
    for workers in 1 2 4 8 16; do
        ./${BUILD_DIR}_${opt_level}/benchmark_scanner $workers "$TARGET_DIR"
    done
    echo ""
}

# Run tests
build_and_run "-O0" "Debug"
build_and_run "-O2" "Release"
build_and_run "-O3" "Release"

echo "Benchmark complete."

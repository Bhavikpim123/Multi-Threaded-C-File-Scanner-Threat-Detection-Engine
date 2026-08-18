#!/bin/bash

TARGET_DIR=${1:-/usr/lib}
WORKERS=${2:-8}
BUILD_DIR="build_perf"

echo "======================================================"
echo "Performance Profiling with perf"
echo "Target Directory: $TARGET_DIR"
echo "Workers: $WORKERS"
echo "======================================================"

# Build with -O3 and debug symbols
cmake -S . -B ${BUILD_DIR} \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_CXX_FLAGS="-O3 -g -fno-omit-frame-pointer" > /dev/null 2>&1

cmake --build ${BUILD_DIR} --target benchmark_scanner > /dev/null 2>&1

if ! command -v perf &> /dev/null; then
    echo "Error: 'perf' is not installed or not in PATH."
    echo "Please install linux-tools-common and the specific linux-tools for your kernel."
    exit 1
fi

echo "Running benchmark_scanner under perf record..."
perf record -g --call-graph dwarf ./${BUILD_DIR}/benchmark_scanner $WORKERS "$TARGET_DIR"

echo ""
echo "perf.data generated."
echo "To explore the hottest functions and bottlenecks, run:"
echo "    perf report"

#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]:-$0}")" && pwd)"

# Detect compiler: honour CXX, then try g++ and clang++.
if [ -z "$CXX" ]; then
    if command -v g++ >/dev/null 2>&1; then
        CXX=g++
    elif command -v clang++ >/dev/null 2>&1; then
        CXX=clang++
    else
        echo "Error: no C++ compiler found. Install g++ or clang++, or set CXX." >&2
        exit 1
    fi
fi

# Verify the header exists next to the test.
if [ ! -f "$SCRIPT_DIR/flat_hash_map.hpp" ]; then
    echo "Error: flat_hash_map.hpp not found in $SCRIPT_DIR" >&2
    echo "Make sure the header file is in the same directory as this script." >&2
    exit 1
fi

echo "Compiler: $CXX"
echo "Compiling test_double_key.cpp ..."
"$CXX" -std=c++14 -O2 -Wall -Wextra -I"$SCRIPT_DIR" \
    "$SCRIPT_DIR/test_double_key.cpp" -o "$SCRIPT_DIR/test_double_key"

echo "Running test_double_key ..."
"$SCRIPT_DIR/test_double_key"

#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CXX="${CXX:-g++}"

echo "Compiling test_double_key.cpp ..."
"$CXX" -std=c++14 -O2 -Wall -Wextra -I"$SCRIPT_DIR" \
    "$SCRIPT_DIR/test_double_key.cpp" -o "$SCRIPT_DIR/test_double_key"

echo "Running test_double_key ..."
"$SCRIPT_DIR/test_double_key"

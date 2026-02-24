#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BIN_PATH="$(mktemp /tmp/floating_price_keys_test.XXXXXX)"
trap 'rm -f "${BIN_PATH}"' EXIT

g++ -std=c++17 -Wall -Wextra -pedantic \
    "${REPO_DIR}/tests/floating_price_keys_test.cpp" \
    -o "${BIN_PATH}"

"${BIN_PATH}"

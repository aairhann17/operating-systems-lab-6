#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

echo "[1/4] Compiling..."
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/main.c src/lab6.c -o lab6

echo "[2/4] Checking built-in SAFE case..."
out1="$(./lab6 < tests/safe_builtin.in)"
echo "$out1" | grep -q "System is in a SAFE state."
echo "$out1" | grep -q "Safe sequence: P1 -> P3 -> P4 -> P0 -> P2"

echo "[3/4] Checking custom UNSAFE case..."
out2="$(./lab6 < tests/unsafe_custom.in)"
echo "$out2" | grep -q "System is in an UNSAFE state."

echo "[4/4] Checking request-granted case..."
out3="$(./lab6 < tests/request_granted_builtin.in)"
echo "$out3" | grep -q "Request GRANTED. System remains SAFE."

echo "All tests passed."

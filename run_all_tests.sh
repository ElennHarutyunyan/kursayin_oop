#!/usr/bin/env bash
set -euo pipefail

COMPILER="${1:-./my_compiler}"
MANIFEST="${2:-./tests/manifest.txt}"

if [[ ! -x "$COMPILER" ]]; then
  echo "Compiler binary not found/executable: $COMPILER"
  echo "Build first: make build"
  exit 1
fi

if [[ ! -f "$MANIFEST" ]]; then
  echo "Manifest not found: $MANIFEST"
  exit 1
fi

total=0
passed=0

while IFS='|' read -r file expected; do
  [[ -z "${file// }" ]] && continue
  total=$((total + 1))
<files/version1.3/run_all_tests.sh" [dos] 34L, 724C           1,1           Top


#!/usr/bin/env bash
set -euo pipefail

COMPILER="${1:-./my_compiler}"
MANIFEST="${2:-./test/manifest.txt}"

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
  [[ "${file#\#}" != "$file" ]] && continue

  total=$((total + 1))
  if "$COMPILER" --test "$file" "$expected"; then
    passed=$((passed + 1))
    echo "[PASS] $file"
  else
    echo "[FAIL] $file (expected: $expected)"
  fi
done < "$MANIFEST"

echo ""
echo "Passed: $passed / $total"
if [[ "$passed" -ne "$total" ]]; then
  exit 1
fi

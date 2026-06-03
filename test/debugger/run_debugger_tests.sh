#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILER="${ROOT}/my_compiler"
DEBUGGER="${ROOT}/my_debugger"
EXE="${ROOT}/a.out.exe"

cd "$ROOT"

if [[ ! -x "$COMPILER" || ! -x "$DEBUGGER" ]]; then
  echo "Build first: make all"
  exit 1
fi

pass=0
total=0

run_case() {
  local name="$1"
  shift
  total=$((total + 1))
  if "$@"; then
    pass=$((pass + 1))
    echo "[PASS] $name"
  else
    echo "[FAIL] $name"
    return 1
  fi
}

run_case "compiler produces executable" test -f "$EXE" || \
  "$COMPILER" --quiet --source test/test01_arithmetic.txt >/dev/null

run_case "debugger rejects bad magic" bash -c "
  printf 'BAD!' > /tmp/bad_magic.exe
  ${DEBUGGER} <<'EOF' 2>&1 | grep -qi 'invalid\|failed'
load -f /tmp/bad_magic.exe
quit
EOF
"

run_case "debugger loads executable" bash -c "
  ${DEBUGGER} <<'EOF' | grep -q 'Loaded'
load -f ${EXE}
quit
EOF
"

run_case "debugger lists functions" bash -c "
  ${DEBUGGER} <<'EOF' | grep -q 'main'
load -f ${EXE}
print -mode functions
quit
EOF
"

run_case "debugger step works" bash -c "
  ${DEBUGGER} <<'EOF' | grep -q 'IP (byte) = 0x'
load -f ${EXE}
step
quit
EOF
"

echo ""
echo "Debugger tests passed: $pass / $total"

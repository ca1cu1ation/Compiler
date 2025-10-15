#词法分析测试脚本

#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./tools/run_lexer_test.sh <input.sy> [expected.lexer] [out.lexer]
# If expected.lexer is omitted, the script will look for testcase/lexer/<basename>.lexer

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT_DIR/bin/compiler"

# Prefer tools/ in PATH so we can provide lightweight shims (e.g. clang-format)
export PATH="$ROOT_DIR/tools:$PATH"

if [ $# -lt 1 ]; then
    echo "Usage: $0 <input.sy> [expected.lexer] [out.lexer]"
    exit 2
fi

INPUT="$1"
EXPECTED="${2:-}"
OUT="${3:-}"

if [ -z "$EXPECTED" ]; then
    EXPECTED="$ROOT_DIR/testcase/lexer/$(basename "$INPUT" .sy).lexer"
fi

if [ -z "$OUT" ]; then
    OUT="$ROOT_DIR/test_output/$(basename "$INPUT" .sy).lexer"
fi

mkdir -p "$(dirname "$OUT")"

if [ ! -x "$BIN" ]; then
    echo "Compiler binary not found at $BIN. Running 'make' to build it..."
    (cd "$ROOT_DIR" && make -j)
fi

if [ ! -f "$INPUT" ]; then
    echo "Input file not found: $INPUT"
    exit 3
fi

echo "Running lexer on: $INPUT"
"$BIN" -lexer -o "$OUT" "$INPUT"

echo "Generated: $OUT"
if [ ! -f "$EXPECTED" ]; then
    echo "Expected file not found: $EXPECTED"
    echo "You can inspect $OUT for token list output." 
    exit 4
fi

echo "Comparing with expected: $EXPECTED"
if diff --strip-trailing-cr -b "$EXPECTED" "$OUT" >/dev/null; then
    echo "PASS: output matches expected"
    exit 0
else
    echo "FAIL: output differs. Showing unified diff:" 
    diff --strip-trailing-cr -u "$EXPECTED" "$OUT" || true
    exit 1
fi

#!/bin/bash
set -e

if command -v x86_64-linux-gnu-gcc &>/dev/null; then
    CC=x86_64-linux-gnu-gcc
else
    CC=gcc
fi
CFLAGS="-O3 -mavx512f"
LDFLAGS=""
[ "$(uname -s)" = "Linux" ] && LDFLAGS="-lrt"
SRC=main.c
OUT=matmul
ASM=matmul.s

echo "=== 3-03-intrinsics-avx512 ==="
$CC $CFLAGS -S -o $ASM $SRC
$CC $CFLAGS -o $OUT $SRC $LDFLAGS

echo "Results for 3-03-intrinsics-avx512 (-O3 -mavx512f)" > results.txt

if [ -x ./$OUT ] && ./$OUT 2>/dev/null; then
    total=0
    for i in $(seq 1 10); do
        output=$(./$OUT)
        t=$(echo "$output" | grep "time:" | awk '{print $2}')
        echo "Run $i: $t s" >> results.txt
        total=$(awk "BEGIN {print $total + $t}")
    done
    avg=$(awk "BEGIN {printf \"%.6f\", $total / 10}")
    echo "Average: $avg s" >> results.txt
else
    echo "Skipping execution (cross-compiled for x86_64, cannot run on this arch)" >> results.txt
    echo "Assembly generated at $ASM" >> results.txt
fi

cat results.txt

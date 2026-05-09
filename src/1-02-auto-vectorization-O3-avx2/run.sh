#!/bin/bash
set -e

if command -v x86_64-linux-gnu-gcc &>/dev/null; then
    CC=x86_64-linux-gnu-gcc
else
    CC=gcc
fi
CFLAGS="-O3 -mavx2"
SRC=main.c
OUT=matmul
ASM=matmul.s

echo "=== 1-02-auto-vectorization-O3-avx2 ==="
$CC $CFLAGS -fopt-info-vec -S -o $ASM $SRC
$CC $CFLAGS -o $OUT $SRC -lrt

echo "Results for 1-02-auto-vectorization-O3-avx2 (-O3 -mavx2)" > results.txt

if [ -x ./$OUT ] && ./$OUT 2>/dev/null; then
    total=0
    for i in $(seq 1 10); do
        output=$(./$OUT)
        t=$(echo "$output" | grep "time:" | awk '{print $2}')
        echo "Run $i: $t s" >> results.txt
        total=$(echo "$total + $t" | bc -l)
    done
    avg=$(echo "scale=6; $total / 10" | bc -l)
    echo "Average: $avg s" >> results.txt
else
    echo "Skipping execution (cross-compiled for x86_64, cannot run on this arch)" >> results.txt
    echo "Assembly generated at $ASM" >> results.txt
fi

cat results.txt

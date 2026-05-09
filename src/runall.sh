#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SUMMARY="$SCRIPT_DIR/all_results.txt"

echo "SIMD Matrix Multiplication - All Results" > "$SUMMARY"
echo "Run date: $(date)" >> "$SUMMARY"
echo "========================================" >> "$SUMMARY"
echo "" >> "$SUMMARY"

for dir in "$SCRIPT_DIR"/*/; do
    name=$(basename "$dir")
    if [ -f "$dir/run.sh" ]; then
        echo "Running $name ..."
        (cd "$dir" && bash run.sh)
        echo "" >> "$SUMMARY"
        echo "--- $name ---" >> "$SUMMARY"
        if [ -f "$dir/results.txt" ]; then
            cat "$dir/results.txt" >> "$SUMMARY"
        else
            echo "No results.txt found" >> "$SUMMARY"
        fi
    fi
done

echo ""
echo "Done. Summary written to $SUMMARY"

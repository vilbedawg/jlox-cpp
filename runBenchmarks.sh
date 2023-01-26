#!/usr/bin/env bash

ROOT_DIR=$(dirname "$0")

JLOX=${ROOT_DIR}/src/main

for SCRIPT_PATH in "${ROOT_DIR}"/tests/benchmarks/*.jlox; do
    SCRIPT=$(basename "${SCRIPT_PATH}" | cut -f 1 -d '.')
    TIME=$(/usr/bin/time -f %e "${JLOX}" "${SCRIPT_PATH}")
    echo "Execution Time for ${SCRIPT} is : ${TIME} seconds"
done

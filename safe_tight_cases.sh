#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

run_test()
{
    printf './codexion'
    printf ' %s' "$@"
    printf '\n'

    output=$(./codexion "$@" 2>&1)
    
    burnouts=$(echo "$output" | grep -c "burned out" || true)

    if [ "$burnouts" -eq 0 ]; then
        printf "${GREEN}OK${NC}\n"
    else
        printf "${RED}BURNOUT (%s)${NC}\n" "$burnouts"
    fi
    printf '\n'

    sleep 0.1
}

# High contention - small groups
run_test 2 850 200 200 200 5 10 fifo
run_test 2 850 200 200 200 5 10 edf

run_test 4 800 200 200 200 5 10 fifo
run_test 4 800 200 200 200 5 10 edf

run_test 5 1000 200 200 200 5 10 fifo
run_test 5 1000 200 200 200 5 10 edf

# Higher contention - more coders
run_test 6 1000 150 150 150 5 10 fifo
run_test 6 1000 150 150 150 5 10 edf

run_test 8 1400 200 200 200 5 20 fifo
run_test 8 1400 200 200 200 5 20 edf

run_test 10 1600 200 200 200 5 20 fifo
run_test 10 1600 200 200 200 5 20 edf

# Cooldown pressure
run_test 4 1000 200 200 200 5 100 fifo
run_test 4 1000 200 200 200 5 100 edf

run_test 5 1200 200 200 200 5 150 fifo
run_test 5 1200 200 200 200 5 150 edf

run_test 8 1600 200 200 200 5 200 fifo
run_test 8 1600 200 200 200 5 200 edf

# Short compilation cycles
run_test 4 500 50 100 100 10 10 fifo
run_test 4 500 50 100 100 10 10 edf

run_test 5 700 50 100 100 10 20 fifo
run_test 5 700 50 100 100 10 20 edf

run_test 8 1000 50 100 100 10 20 fifo
run_test 8 1000 50 100 100 10 20 edf

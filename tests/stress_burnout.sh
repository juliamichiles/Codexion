#!/bin/bash
cmd=(./codexion 4 800 200 200 200 5 10 fifo)

if (($#)); then
  if [[ $1 == "--" ]]; then
    shift
  fi
  if (($# == 0)); then
    echo "Usage: $0 [-- command [args...]]" >&2
    exit 2
  fi
  cmd=("$@")
fi

mkdir -p fails
for i in $(seq 300); do
  "${cmd[@]}" > out.txt
  if grep -q "burned out" out.txt; then cp out.txt fails/run_$i.txt; fi
done
ls fails

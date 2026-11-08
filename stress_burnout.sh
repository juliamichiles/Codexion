mkdir -p fails
for i in $(seq 300); do
  ./codexion 4 800 200 200 200 5 10 fifo > out.txt
  if grep -q "burned out" out.txt; then cp out.txt fails/run_$i.txt; fi
done
ls fails

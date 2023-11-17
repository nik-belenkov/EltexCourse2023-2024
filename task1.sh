#!/bin/bash

cnt=0
echo "Run sctipt"
for i in {1..5};
  do
    echo "Running process $i"
    sleep 10;
    echo "Process $i finished"
    cnt=$(( $cnt + 1))
  done;
  
echo "Fineshed processes = $cnt" > log.txt
sleep 3600;

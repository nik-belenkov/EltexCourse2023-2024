#!/bin/bash

for i in {1..3};
  do
    kill `ps au | grep 'sleep 10' | awk '{print $2}'`
  done;

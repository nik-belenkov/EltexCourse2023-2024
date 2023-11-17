#!/bin/bash

kill -9 `ps au | grep 'task1' | awk '{print $2}'`

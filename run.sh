#!/bin/bash
gcc random.c new.c -lm -o cnew
gcc random.c upgrade.c -lm -o cupgrade
gcc random.c repair.c -lm -o crepair
gcc random.c sorter.c -lm -o sorter
gcc stopwatch.c random.c main.c  -lm -o main
./main

#!/usr/bin/python3
import sys

for line in sys.stdin:
    line = line.strip().split()
    print(line[0], ' ', line[1], ' ', line[2], ' ', line[3])

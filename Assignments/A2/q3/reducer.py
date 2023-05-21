#!/usr/bin/python3
import sys

count = 0
total = 0.0

for line in sys.stdin:
    line = line.strip()
    val , k = line.split('\t')
    # print([val, k])
    total = total + (int(val) * (int)(k))
    count += int(k)

print(total/count)
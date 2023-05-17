#!/usr/bin/python3
import random
import sys

n_iterations = 10

for line in sys.stdin:
    val = line.strip()
    if (val == ""):
        pass
    else:
        n_iterations = int(val)
        break


def CountFreq(li):
    freq = {}
    for item in li:
        if (item in freq):
            freq[item] += 1
        else:
            freq[item] = 1
    return (freq)


# max_val = 10000
num_list = []
for i in range(n_iterations):
    sum = 0.0
    k = 0
    count = 0
    # while sum <= 1.0 and count <= max_val:
    while sum <= 1.0:
        num = random.uniform(0, 1)
        sum += num
        count += 1
    num_list.append(count)

freq_dict = CountFreq(num_list)
for key in freq_dict:
    print(key, '\t', freq_dict[key])

#!/usr/bin/python3
import sys

lines = sys.stdin.readlines()
# print(lines)
# print(len(lines))

# curr_i = 0
# curr_j = 0
# temp_ans = 0

curr_i, curr_j = 0, 0
temp_ans = 0
step = 2
flag = True

for k in range(0, len(lines), step):
    # print(lines[k])
    # print(lines[k+1])
    val1 = int(lines[k].strip().split()[3])
    val2 = int(lines[k+1].strip().split()[3])
    # print([val1, val2])

    item_list = lines[k+1].strip().split()
    i = (int)(item_list[0])
    j = (int)(item_list[1])
    pos = (int)(item_list[2])
    value = (int)(item_list[3])

    # print([i,j,pos,value])

    # i, j, pos, val = list(map(int,lines[k+1].strip().split()))

    if (i == curr_i and j == curr_j):
        temp_ans += val1*val2
    else:
        if (flag == True):
            print(temp_ans, end="")
            flag = False
        else:
            print(" ", end = "")
            print(temp_ans, end = "")
        # print(temp_ans, end = " ")
        temp_ans = 0
        temp_ans = val1*val2
        if i != curr_i:
            flag = True
            c = 0
            print()
        curr_i, curr_j = i, j
if (flag == True):
    print(temp_ans, end="")
else:
    print(" ", end = "")
    print(temp_ans, end="")
# print(temp_ans)

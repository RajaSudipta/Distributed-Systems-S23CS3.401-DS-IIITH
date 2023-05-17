#!/usr/bin/python3
import sys
import os

local_input_file_path = sys.argv[1]

m_r = 0
m_c = 0
n_r = 0
n_c = 0


def pad_leading_zeros(num):
    string = str(num)
    string_padded = string.rjust(4, "0")
    return string_padded


def calculate_dimension(lines):
    m_r = (int)(lines[0].strip().split()[0])
    m_c = (int)(lines[0].strip().split()[1])
    n_r = (int)(lines[m_r+1].strip().split()[0])
    n_c = (int)(lines[m_r+1].strip().split()[1])
    return m_r, m_c, n_r, n_c


f1 = open(local_input_file_path, 'r')
lines = f1.readlines()
f1.close()
m_r, m_c, n_r, n_c = calculate_dimension(lines)

i = 0
count = 0
matrix1 = []
matrix2 = []
for line in lines:
    line = line.strip().split()
    if ((i == 0) or (i == m_r+1)):
        count += 1
    else:
        if i <= m_r:
            for j in range(len(line)):
                for k in range(n_c):
                    matrix1.append(
                        [pad_leading_zeros(i-1), pad_leading_zeros(k), pad_leading_zeros(j), line[j]])
        else:
            for j in range(len(line)):
                for k in range(m_r):
                    matrix2.append([pad_leading_zeros(k), pad_leading_zeros(
                        j), pad_leading_zeros(i-(m_r+2)), line[j]])
    i += 1

f2 = open('intermediate.txt', 'w')
for elem in matrix1:
    string = " ".join(str(s) for s in elem)
    f2.write(string)
    f2.write("\n")

for elem in matrix2:
    string = " ".join(str(s) for s in elem)
    f2.write(string)
    f2.write("\n")
f2.close()

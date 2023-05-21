#!/usr/bin/python3
import sys
from collections import Counter

dict_a = {}
dict_b = {}

for line in sys.stdin:
    # print(line)
    words_list = line.split()
    # print(words_list)
    # Matrix A
    if (words_list[0] == '0'):
        row = (int)(words_list[1])
        col = (int)(words_list[2])
        val = (int)(words_list[3])
        dict_a.setdefault(row, []).append([col, val])
    # Matrix B
    elif (words_list[0] == '1'):
        row = (int)(words_list[1])
        col = (int)(words_list[2])
        val = (int)(words_list[3])
        dict_b.setdefault(row, []).append([col, val])

# print(dict_a)
# print(dict_b)


dict_result = {}
row_list_result = []
col_list_result = []
for key in dict_a:
    m1_row = key
    m1_col_val_list = dict_a[m1_row]
    # print(m1_col_val_list)
    for elem in m1_col_val_list:
        m1_col = elem[0]
        m1_val = elem[1]
        # print([m1_col, m1_val])
        for key in dict_b:
            m2_row = key
            if (m1_col == m2_row):
                m2_col_val_list = dict_b[m2_row]
                # print(m2_col_val_list)
                for elem2 in m2_col_val_list:
                    m2_col = elem2[0]
                    m2_val = elem2[1]
                    resultant_row = m1_row
                    resultant_col = m2_col
                    resultant_val = m1_val * m2_val
                    row_list_result.append(resultant_row)
                    col_list_result.append(resultant_col)
                    if (resultant_row, resultant_col) in dict_result.keys():
                        dict_result[(resultant_row, resultant_col)
                                    ] += resultant_val
                    else:
                        dict_result[(resultant_row, resultant_col)
                                    ] = resultant_val

# print(dict_result)

n_rows_result = len(Counter(row_list_result).keys())
n_cols_result = len(Counter(col_list_result).keys())

for i in range(n_rows_result):
    for j in range(n_cols_result):
        print(dict_result[(i,j)], end=" ")
    print()

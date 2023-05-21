#!/usr/bin/python3
import sys
import fileinput

m_r = 0
m_c = 0
n_r = 0
n_c = 0

c = 0
data_list = []
for line in sys.stdin:
    # print(line)
    words_list = line.split()
    # print(words_list)
    if (c == 0):
        m_r = (int)(words_list[0])
        m_c = (int)(words_list[1])
    elif (c == m_r+1):
        n_r = (int)(words_list[0])
        n_c = (int)(words_list[1])
    else:
        data_list.append(words_list)
    c += 1

# print("m_r: " + str(m_r))
# print("m_c: " + str(m_c))
# print("n_r: " + str(n_r))
# print("n_c: " + str(n_c))

i = 0
c = 0
for line in data_list:
	# print(line)
	if (i < m_r):
		for j in range(len(line)):
			print("{0} {1} {2} {3}".format(0, i, j, line[j]))
	else:
		for j in range(len(line)):
			print("{0} {1} {2} {3}".format(1, i-m_r, j, line[j]))
	i+=1

# i = 0
# c = 0
# for line in sys.stdin:
#     if (not ((c == 0) or (c == m_r+1))):
#         words_list = line.split()
#         # print(words_list)
#         if (i < m_r):
#             for j in range(len(words_list)):
#                 print("{0} {1} {2} {3}".format(0, i, j, words_list[j]))
#         else:
#             for j in range(len(words_list)):
#                 print("{0} {1} {2} {3}".format(1, i-m_r, j, words_list[j]))
#         i+=1
#     c+=1
        
# i = 0
# c = 0
# file1 = open('input.txt', 'r')
# for line in file1:
# 	words_list = line.split()
# 	if(not((c == 0) or (c == m_r+1))):
# 		# print(words_list)
# 		if(i < m_r):
# 			for j in range(len(words_list)):
# 				for k in range(n_c):
# 					print("{0} {1} {2} {3}".format(i, k, j, words_list[j]))
# 		else:
# 			for j in range(len(words_list)):
# 				for k in range(m_r):
# 					print("{0} {1} {2} {3}".format(k, j, i-m_r, words_list[j]))
# 		i = i+1
# 	c += 1
# file1.close()

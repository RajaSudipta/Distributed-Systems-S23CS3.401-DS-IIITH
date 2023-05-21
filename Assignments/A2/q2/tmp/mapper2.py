import sys

m_r = 0
m_c = 0
n_r = 0
n_c = 0

out_file = open('intermediate.txt', 'w')

c = 0
f1 = open('input.txt', 'r')
for line in f1:
	words_list = line.split()
	if(c == 0):
		m_r = (int)(words_list[0])
		m_c = (int)(words_list[1])
	if(c == m_r+1):
		n_r = (int)(words_list[0])
		n_c = (int)(words_list[1])
	c += 1

f1.close()

# print("m_r: " + str(m_r))
# print("m_c: " + str(m_c))
# print("n_r: " + str(n_r))
# print("n_c: " + str(n_c))

out_file.write("{0} {1}\n".format(m_r, m_c))
out_file.write("{0} {1}\n".format(n_r, n_c))

# print("{0} {1}".format(m_r, m_c))
# print("{0} {1}".format(n_r, n_c))


i = 0
c = 0
file1 = open('input.txt', 'r')
for line in file1:
	words_list = line.split()
	if(not((c == 0) or (c == m_r+1))):
		# print(words_list)
		if(i < m_r):
			for j in range(len(words_list)):
				for k in range(n_c):
					# print("{0} {1} {2} {3}".format(i, k, j, words_list[j]))
					out_file.write("{0} {1} {2} {3}\n".format(i, k, j, words_list[j]))
		else:
			for j in range(len(words_list)):
				for k in range(m_r):
					# print("{0} {1} {2} {3}".format(k, j, i-m_r, words_list[j]))
					out_file.write("{0} {1} {2} {3}\n".format(k, j, i-m_r, words_list[j]))
		i = i+1
	c += 1
file1.close()

out_file.close()
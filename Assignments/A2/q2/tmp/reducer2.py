import sys

file1 = open('intermediate.txt', 'r')

ln = file1.readline()
wl = ln.split()
print(wl)
m_r = (int)(wl[0])
m_c = (int)(wl[1])
ln = file1.readline()
wl = ln.split()
print(wl)
n_r = (int)(wl[0])
n_c = (int)(wl[1])

print("m_r: " + str(m_r))
print("m_c: " + str(m_c))
print("n_r: " + str(n_r))
print("n_c: " + str(n_c))

matrix = []
for row in range(m_r):
	r = []
	for col in range(n_c):
		s = 0
		for el in range(m_c):
			mul = 1
			for num in range(2):
				line = file1.readline()
				words_list = line.split()
				print(words_list)
				n = (int)(words_list[-1])
				# n=map(int,line.split(''))[-1]
				mul *= n
			s+=mul
		r.append(s)
	matrix.append(r)
print('\n'.join([str(x) for x in matrix]))
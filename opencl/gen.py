#!/usr/bin/python3

from random import randrange
from sys import argv, stderr

if len(argv) < 9:
	print("i, j, min_i, max_i, min_j, max_j, minv, maxv")
	exit(1)


i = int(argv[1])
j = int(argv[2])
min_i = int(argv[3])
max_i = int(argv[4])
min_j = int(argv[5])
max_j = int(argv[6])
min_v = int(argv[7])
max_v = int(argv[8])+1

tot = i*j

for ii in range(i):
	for jj in range(j):
		if ii < min_i or ii > max_i or jj < min_j or jj > max_j:
			val = 0
		else:
			val = min_v+randrange(max_v-min_v)

		print('{} {} {}'.format(ii, jj, val))
		print('{:.3f}%\r'.format((ii*j+jj)/tot*100), flush=True, file=stderr, end='')

#!/usr/bin/python3

from random import randrange
from sys import argv, stderr

if len(argv) < 5:
	print("i, j, minv, maxv, p")
	exit(1)


i = int(argv[1])
j = int(argv[2])
minv = int(argv[3])
maxv = int(argv[4])+1
p = int(argv[4])

tot = i*j

for ii in range(i):
	for jj in range(j):
		if randrange(p) == 0:
			val = minv+randrange(maxv-minv)
		else:
			val = 0
		print('{} {} {}'.format(ii, jj, val))
		print('{:.3f}%\r'.format((ii*j+jj)/tot*100), flush=True, file=stderr, end='')

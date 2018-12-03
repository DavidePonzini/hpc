#!/usr/bin/python3

from random import randrange
from sys import argv

if len(argv) < 5:
	print("i, j, minv, maxv, p")
	exit(1)


i = int(argv[1])
j = int(argv[2])
minv = int(argv[3])
maxv = int(argv[4])+1
p = int(argv[4])

for ii in range(i):
	for jj in range(j):
		if randrange(p) == 0:
			val = minv+randrange(maxv-minv)
		else:
			val = 0
		print('{} {} {}'.format(ii, jj, val))

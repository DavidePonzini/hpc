#!/usr/bin/python3

from random import randrange
from sys import argv

maxv = int(argv[2])+1
minv = int(argv[1])
n = int(argv[3])
p = int(argv[4])

for i in range(n):
	for j in range(n):
		if randrange(p) == 0:
			val = minv+randrange(maxv-minv)
		else:
			val = 0
		print('{} {} {}'.format(i, j, val))

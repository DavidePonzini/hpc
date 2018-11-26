#!/bin/python3

from random import randrange
from sys import argv

maxv = int(argv[2])+1
minv = int(argv[1])
n = int(argv[3])

#print('min: {}, max: {}, n: {}'.format(minv, maxv, n))

for i in range(n):
	for j in range(n):
		print('{} {} {}'.format(i, j, minv+randrange(maxv-minv)))

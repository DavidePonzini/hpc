#!/bin/bash -e

for t in 1 2 4 8 16; do

	export OMP_NUM_THREADS=$t

	echo "Threads: $OMP_NUM_THREADS"
	> mergesort_${OMP_NUM_THREADS}.dat

	while read i; do
		echo "Size: $i"
		g++ -O3 -fopenmp -DSIZE=$i mergesort.cpp -o mergesort
		./mergesort >> mergesort_${OMP_NUM_THREADS}.dat
	done < logspace
done

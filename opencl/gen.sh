#!/bin/bash -e

n=$1
n=$((n-1))
x=$2

for i in `seq 0 $n`; do
	for j in `seq 0 $n`; do
		echo "$i $j $x"
		x=$((x+1))
	done
done

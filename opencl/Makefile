CC=g++
CC_OPTS=

BLOCK=1

SIZE?=10
K?=0.1
D?=10
C?=.1
STEP?=1
DT?=1
TIME?=1
IN?=in
OUT?=out
STEPS?=1

SCRIPT?=script.sh
LOG?=log
ERR?=err

plot: run
	gnuplot plot.gp

run: compile
	./main $(SIZE) $(K) $(D) $(C) $(STEP) $(DT) $(TIME) $(IN) $(OUT)

compile: *.cpp *.h
	$(CC) -std=c++11 -O3 -o main $(CC_OPTS) *.cpp

compile2: main2.cpp file.cpp file.h
	$(CC) -std=c++11 -lOpenCL -O3 -o main2 $(CC_OPTS) main2.cpp file.cpp -DBLOCK=$(BLOCK)

run2: compile2
	echo '#!/bin/bash' > $(SCRIPT)
	echo "`pwd`/main2 $(SIZE) $(K) $(D) $(C) $(STEP) $(DT) $(TIME) `pwd`/in `pwd`/out" > $(SCRIPT)

	qsub -o $(LOG) -e $(ERR) $(SCRIPT)

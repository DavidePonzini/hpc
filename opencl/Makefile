CC=g++
CC_OPTS=

COLS?=15
ROWS?=15
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


cpu: main
	./main $(ROWS) $(COLS) $(K) $(D) $(C) $(STEP) $(DT) $(TIME) $(IN) $(OUT)

%: %.cpp file.cpp file.h
	$(CC) -std=c++11 -lOpenCL -O3 -o $@ $(CC_OPTS) $@.cpp file.cpp

gpu: main2
	echo '#!/bin/bash' > $(SCRIPT)
	echo "`pwd`/main2 $(ROWS) $(COLS) $(K) $(D) $(C) $(STEP) $(DT) $(TIME) `pwd`/$(IN) `pwd`/$(OUT)" > $(SCRIPT)

	qsub -o $(LOG) -e $(ERR) $(SCRIPT)

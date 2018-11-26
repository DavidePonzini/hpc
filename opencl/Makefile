CC=g++
CC_OPTS=


SIZE?=3
K?=1
D?=1
C?=1
STEP?=1
DT?=1
TIME?=1
IN?=in
OUT?=out
STEPS?=1

run: compile
	./main $(SIZE) $(K) $(D) $(C) $(STEP) $(DT) $(TIME) $(IN) $(OUT)

compile: *.cpp
	$(CC) -std=c++11 -O3 -o main $(CC_OPTS) *.cpp


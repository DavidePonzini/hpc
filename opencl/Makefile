CC=g++
CC_OPTS=


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


plot: run
	gnuplot plot.gp

run: compile
	./main $(SIZE) $(K) $(D) $(C) $(STEP) $(DT) $(TIME) $(IN) $(OUT)

compile: *.cpp *.h
	$(CC) -std=c++11 -O3 -o main $(CC_OPTS) *.cpp


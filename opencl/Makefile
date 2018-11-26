CC=g++
CC_OPTS=
STEPS?=1

run: compile
	./main 6 1 1 1 2 1 $(STEPS) in out

compile: *.cpp
	$(CC) -std=c++11 -O3 -o main $(CC_OPTS) *.cpp


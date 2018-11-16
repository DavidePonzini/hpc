n=?2

all: compile run

compile:
	mpic++ -std=c++11 -o pingpong ping_pong.cpp
run:
	mpiexec -n $(n) -machinefile ../nodelist ./pingpong 0 0

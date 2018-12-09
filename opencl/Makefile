CC=g++
CC_OPTS=

COLS?=15
ROWS?=15
K?=0.0001
D?=1000
C?=1
L?=1
DT?=1
TIME?=1
IN?=in
OUT?=out
STEPS?=1

SCRIPT?=script.sh
LOG?=log
ERR?=err


cpu: main_cpu
	echo '#!/bin/bash' > $(SCRIPT)
	echo "`pwd`/main_cpu $(ROWS) $(COLS) $(K) $(D) $(C) $(L) $(DT) $(TIME) `pwd`/$(IN) `pwd`/$(OUT)_cpu_$(TIME)" > $(SCRIPT)

	qsub -o log/cpu/$(LOG)_$(IN)_cpu_$(TIME) -e err/cpu/$(ERR)_$(IN)_cpu_$(TIME) $(SCRIPT)

gpu_g: main_gpu_global
	echo '#!/bin/bash' > $(SCRIPT)
	echo "`pwd`/main_gpu_global $(ROWS) $(COLS) $(K) $(D) $(C) $(L) $(DT) $(TIME) `pwd`/$(IN) `pwd`/$(OUT)_gpu_g_$(TIME)" > $(SCRIPT)

	qsub -o log/global/$(LOG)_$(IN)_gpu_$(TIME) -e err/global/$(ERR)_$(IN)_gpu_$(TIME) $(SCRIPT)

gpu_l: main_gpu_local
	echo '#!/bin/bash' > $(SCRIPT)
	echo "`pwd`/main_gpu_local $(ROWS) $(COLS) $(K) $(D) $(C) $(L) $(DT) $(TIME) `pwd`/$(IN) `pwd`/$(OUT)_gpu_l_$(TIME)" > $(SCRIPT)

	qsub -o log/local/$(LOG)_$(IN)_gpu_$(TIME) -e err/local/$(ERR)_$(IN)_gpu_$(TIME) $(SCRIPT)

%: %.cpp file.cpp file.h
	$(CC) -std=c++11 -lOpenCL -O3 -o $@ $(CC_OPTS) $@.cpp file.cpp


clean:
	rm -vf log/cpu/* err/cpu/*
	rm -vf log/local/* err/local/*
	rm -vf log/global/* err/global/*

	rm -vf small_out* medium_out* large_out* xxl_out
	rm -vf small_out* main_cpu main_gpu_global main_gpu_local

#!/bin/bash

make clean

#make cpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=1 CC_OPTS='-DTRIALS=1'
#make cpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=10 CC_OPTS='-DTRIALS=1'
#make cpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=100 CC_OPTS='-DTRIALS=1'

make gpu_l ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=1 CC_OPTS='-DTRIALS=1'
make gpu_l ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=2 CC_OPTS='-DTRIALS=1'
make gpu_l ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=3 CC_OPTS='-DTRIALS=1'
make gpu_l ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=4 CC_OPTS='-DTRIALS=1'
make gpu_l ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=5 CC_OPTS='-DTRIALS=1'
make gpu_l ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=10 CC_OPTS='-DTRIALS=1'
make gpu_l ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=100 CC_OPTS='-DTRIALS=1'
make gpu_l ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=1000 CC_OPTS='-DTRIALS=1'
#make gpu_l ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=10000 CC_OPTS='-DTRIALS=1'
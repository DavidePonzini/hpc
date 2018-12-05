#!/bin/bash

#make cpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=1 CC_OPTS='-DTRIALS=1'
#make cpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=10 CC_OPTS='-DTRIALS=1'
#make cpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=100 CC_OPTS='-DTRIALS=1'

make gpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=1 CC_OPTS='-DTRIALS=1'
make gpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=10 CC_OPTS='-DTRIALS=1'
make gpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=100 CC_OPTS='-DTRIALS=1'
make gpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=1000 CC_OPTS='-DTRIALS=1'
make gpu ROWS=.1 COLS=.1 L=0.001 IN=small_in.dat OUT=small_out.dat TIME=10000 CC_OPTS='-DTRIALS=1'

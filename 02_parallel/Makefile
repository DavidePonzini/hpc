CC=g++
CC_OPTS=
OMP_NUM_THREADS?=2


run: \
	run_matmul_loop_tiling_parallel_inner \
	run_matmul_loop_tiling_parallel_outer \
	run_matmul_loop_exchange_parallel_inner \
	run_matmul_loop_exchange_parallel_outer
	

compile: \
	matmul_loop_tiling_parallel_inner \
	matmul_loop_tiling_parallel_outer \
	matmul_loop_exchange_parallel_inner \
	matmul_loop_exchange_parallel_outer

%: %.cpp
	$(CC) $< -O3 -o $@ -fopenmp $(CC_OPTS)

clean:
	rm -f matmul_loop_tiling_parallel_inner 
	rm -f matmul_loop_tiling_parallel_outer 
	rm -f matmul_loop_exchange_parallel_inner 
	rm -f matmul_loop_exchange_parallel_outer
	
test_run: compile
	./matmul_loop_tiling_parallel_inner 300 300 3
	./matmul_loop_tiling_parallel_outer 300 300 3
	./matmul_loop_exchange_parallel_inner 300 300 3
	./matmul_loop_exchange_parallel_outer 300 300 3
	
run_%: compile
	./$* 16 1000 4 > $*_T$(OMP_NUM_THREADS).dat
	./$* 1100 3000 100 >> $*_T$(OMP_NUM_THREADS).dat
	./$* 4000 6000 1000 >> $*_T$(OMP_NUM_THREADS).dat


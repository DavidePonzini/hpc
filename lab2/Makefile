CC=g++
CC_OPTS=
T?=2

compile: matmul_block_access_parallel_inner matmul_block_access_parallel_outer matmul_loop_exchange_parallel_inner matmul_loop_exchange_parallel_outer

%: %.cpp
	$(CC) $< -O3 -o $@ -fopenmp $(CC_OPTS)

set_threads:
	export OMP_NUM_THREADS=$(T)
	
clean:
	rm -f matmul_block_access_parallel_inner 
	rm -f matmul_block_access_parallel_outer 
	rm -f matmul_loop_exchange_parallel_inner 
	rm -f matmul_loop_exchange_parallel_outer
	
test_run: compile
	./matmul_block_access_parallel_inner 300 300 3
	./matmul_block_access_parallel_outer 300 300 3
	./matmul_loop_exchange_parallel_inner 300 300 3
	./matmul_loop_exchange_parallel_outer 300 300 3
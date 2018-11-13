set terminal png size 1920,1080 font b018015l 14
set output "matmul-performance_loop_tiling.png"

set style data lines
set xlabel "matrix size" offset -1,0
set xrange [10 : 6100]
set ylabel "performance (GFLOPS)" offset 0,-1
set yrange [0 : 8]

plot \
"../../lab1/data/loop_tiling_intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O3.dat" t "Loop tiling - sequential",\
"matmul_loop_tiling_parallel_inner_T2.dat" t "Loop tiling - inner loop - 2 threads",\
"matmul_loop_tiling_parallel_outer_T2.dat" t "Loop tiling - outer loop - 2 threads"


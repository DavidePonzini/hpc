set terminal png size 1920,1080 font b018015l 14
set output "matmul-performance.png"

set style data lines
set xlabel "matrix size" offset -1,0
set xrange [10 : 6100]
set ylabel "performance (GFLOPS)" offset 0,-1
set yrange [0 : 6]

plot \
"o0.dat" t "Intel Core i5 1.8GHz, 256KB L2 cache, DDR3 1600 MHz, gcc 7.3.0 -O0",\
"o1.dat" t "Intel Core i5 1.8GHz, 256KB L2 cache, DDR3 1600 MHz, gcc 7.3.0 -O1",\
"o2.dat" t "Intel Core i5 1.8GHz, 256KB L2 cache, DDR3 1600 MHz, gcc 7.3.0 -O2",\
"o3.dat" t "Intel Core i5 1.8GHz, 256KB L2 cache, DDR3 1600 MHz, gcc 7.3.0 -O3"


set terminal png size 1920,1080 font b018015l 14
set output "matmul-performance.png"

set style data lines
set xlabel "matrix size" offset -1,0
set xrange [10 : 6100]
set ylabel "performance (GFLOPS)" offset 0,-1
set yrange [0 : 3]

plot \
"intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O0.dat" t "Intel Core i5-650 3.2GHz, 256KB L2 cache, DDR3 662 MHz, gcc 7.3.0 -O0",\
"intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O1.dat" t "Intel Core i5-650 3.2GHz, 256KB L2 cache, DDR3 662 MHz, gcc 7.3.0 -O1",\
"intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O2.dat" t "Intel Core i5-650 3.2GHz, 256KB L2 cache, DDR3 662 MHz, gcc 7.3.0 -O2",\
"intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O3.dat" t "Intel Core i5-650 3.2GHz, 256KB L2 cache, DDR3 662 MHz, gcc 7.3.0 -O3"


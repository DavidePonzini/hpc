set terminal png size 1920,1080 font b018015l 14
set output "mergesort_performance.png"

set style data lines
set xlabel "matrix size" offset -1,0
#set xrange [10 : 6100]
set ylabel "performance (min)" offset 0,-1
#set yrange [0 : 2]
set logscale xy

plot \
"mergesort_1.dat" t "Mergesort - 1 thread", \
"mergesort_2.dat" t "Mergesort - 2 threads", \
"mergesort_4.dat" t "Mergesort - 4 threads", \
"mergesort_8.dat" t "Mergesort - 8 threads", \
"mergesort_16.dat" t "Mergesort - 16 threads"


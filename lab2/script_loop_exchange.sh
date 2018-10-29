for i in 0 1 2 3 ; do
 g++ -std=c++11 -O$i -o mm_$i matmul_loop_exchange.cpp || exit;
 ./mm_$i 16 1000 4 > loop-ex_intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O$i.dat
 ./mm_$i 1100 3000 100 >> loop-ex_intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O$i.dat
 ./mm_$i 4000 6000 1000 >> loop-ex_intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O$i.dat
done

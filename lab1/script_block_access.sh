for i in 0 1 2 3 ; do
 g++ -std=c++11 -O$i -o mm_$i matmul_block_access.cpp || exit;
 ./mm_$i 16 1000 4 > o$i.dat
 ./mm_$i 1100 3000 100 >> o$i.dat
 ./mm_$i 4000 6000 1000 >> o$i.dat
done

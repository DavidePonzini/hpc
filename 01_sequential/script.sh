# prefix variable used for runnig alternate versions (loop exchange/tiling)
#   without modifying the script
# can either be:
# prefix=							# normal version
# prefix=loop_exchange_		# loop exchange
# prefix=loop_tiling_			# loop tiling


for i in 0 1 2 3 ; do
	g++ -std=c++11 -O$i -o mm_$i ${prefix}matmul.cpp || exit;
	./mm_$i 16 1000 4 > ${prefix}intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O$i.dat
	./mm_$i 1100 3000 100 >> ${prefix}intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O$i.dat
	./mm_$i 4000 6000 1000 >> ${prefix}intel-corei5-650-3.2GHz_DDR3-662MHz_gcc730-O$i.dat
done

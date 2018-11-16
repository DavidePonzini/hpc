> out
mpic++ -std=c++11 -o pingpong ping_pong.cpp

for n in `seq 1 36`; do
	echo $n
	mpiexec -n $n -machinefile ../nodelist ./pingpong 0 0 >> out
done

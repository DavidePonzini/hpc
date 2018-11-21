set terminal postscript landscape color solid "Helvetica" 14
#set output 'matmul-1.ps'
#set output 'matmul-2.ps'
#set output 'matmul-3.ps'
#set output 'matmul-4.ps'
#set output 'matmul-5.ps'
set output 'matmul-6.ps'
set noclip points
set clip one
set noclip two
set border
set boxwidth
set dummy x,y
set format x "%g"
set format y "%g"
set format z "%g"
set nogrid
set nolabel
set noarrow
set nologscale
set offsets 0, 0, 0, 0
set nopolar
set angles radians
set noparametric
set view 60, 30, 1, 1
set samples 100, 100
set isosamples 10, 10
set surface
set nocontour
set clabel
set nohidden3d
set cntrparam order 4
set cntrparam linear
set cntrparam levels auto 5
set cntrparam points 5
set xzeroaxis
set yzeroaxis
set tics in
set ticslevel 0.5
set xtics
set ytics
set ztics
#set title "" 0,0
set notime
set rrange [0 : 10]
set trange [-5 : 5]
set urange [-5 : 5]
set vrange [-5 : 5]
set size 1.1,1.1
set style data lines
set style function dots
set xlabel "Ordine matrici" offset 0,-1
#set xrange [10 : 7000]
#set xrange [10 : 17000]
#set xrange [10 : 17000]
#set xrange [10 : 17000]
#set xrange [10 : 17000]
set xrange [10 : 17000]
set ylabel "Performance (GFLOPS)" offset -1,0
#set yrange [0 : 160]
#set key at 6000,155
#set yrange [0 : 300]
#set key at 16000,200
#set yrange [0 : 450]
#set key at 16000,440
#set yrange [0 : 1100]
#set key at 16000,850
#set yrange [0 : 400]
#set key at 16000,390
set yrange [0 : 1700]
set key at 16000,1200
set zlabel "" offset 0,0
set zrange [-10 : 10]
set autoscale r
set autoscale t
set noautoscale   
set autoscale z
set zero 1e-08

#plot\
#"gpu_amd-radeon-r9-390_cl_global_no-data-ovhead.dat" t "OpenCL global mem w/o data overhead, Radeon R9 390, 2560 PEs", \
#"gpu_amd-radeon-r9-390_cl_global.dat" t "OpenCL global mem, Radeon R9 390, 2560 PEs", \
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O3_xchg_2x8cores.dat" t "Intel Xeon E5 2650 2.6 GHz, loop xchg, 2x8 cores",\
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O3_xchg_6cores.dat" t "Intel Xeon E5 2650 2.6 GHz, loop xchg, 6 cores",\
#"corei7-5820k_3.3GHz_16MBL2_DDR43000_gcc530-O3_xchg_6cores.dat" t "Intel i7 5820k 3.3 GHz, loop xchg, 6 cores",\
#"cpu_cl_global.dat" t "OpenCL global mem, CPU device Intel i7 5820k 3.3 GHz, 6 cores",\
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O3_xchg_1cores.dat" t "Intel Xeon E5 2650 2.6 GHz, loop xchg, 1 core",\
#"corei7-5820k_3.3GHz_16MBL2_DDR43000_gcc530-O3_xchg_1cores.dat" t "Intel i7 5820k 3.3 GHz, loop xchg, 1 core"

#plot\
#"gpu_amd-radeon-r9-390_cl_local_no-data-ovhead.dat" t "OpenCL local mem w/o data overhead, Radeon R9 390, 2560 PEs",\
#"gpu_amd-radeon-r9-390_cl_local.dat" t "OpenCL local mem, Radeon R9 390, 2560 PEs",\
#"gpu_amd-radeon-r9-390_cl_global.dat" t "OpenCL global mem, Radeon R9 390, 2560 PEs",\
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O3_xchg_2x8cores.dat" t "Intel Xeon E5 2650 2.6 GHz, loop xchg, 2x8 cores",\
#"corei7-5820k_3.3GHz_16MBL2_DDR43000_gcc530-O3_xchg_6cores.dat" t "Intel i7 5820k 3.3 GHz, loop xchg, 6 cores"

#plot\
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O2_blas_mkl_2x8cores.dat" t "Intel Xeon E5 2650 2.6 GHz, MKL BLAS, 2x8 cores",\
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O2_blas_openblas_2x8cores.dat" t "Intel Xeon E5 2650 2.6 GHz, OpenBLAS, 2x8 cores",\
#"corei7-5820k_3.3GHz_16MBL2_DDR43000_gcc530-O3_blas_openblas_6cores.dat" t "Intel i7 5820k 3.3 GHz, OpenBLAS, 6 cores", \
#"gpu_amd-radeon-r9-390_cl_local_no-data-ovhead.dat" t "OpenCL local mem w/o data overhead, Radeon R9 390, 2560 PEs",\
#"gpu_amd-radeon-r9-390_cl_local.dat" t "OpenCL local mem, Radeon R9 390, 2560 PEs", \
#"gpu_amd-radeon-r9-390_cl_global.dat" t "OpenCL global mem, Radeon R9 390, 2560 PEs",\
#"corei7-5820k_3.3GHz_16MBL2_DDR43000_gcc530-O3_blas_openblas_1cores.dat" t "Intel i7 5820k 3.3 GHz, OpenBLAS, 1 core"

#plot\
#"gpu_nvidia-tesla-k40c_cublas.dat" t "CUBLAS, nVIDIA Tesla K40c, 2880 PEs", \
#"gpu_amd-radeon-r9-390_clblas.dat" t "clBLAS, Radeon R9 390, 2560 PEs", \
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O2_blas_mkl_2x8cores.dat" t "Intel Xeon E5 2650 2.6 GHz, MKL BLAS, 2x8 cores",\
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O2_blas_openblas_2x8cores.dat" t "Intel Xeon E5 2650 2.6 GHz, OpenBLAS, 2x8 cores",\
#"corei7-5820k_3.3GHz_16MBL2_DDR43000_gcc530-O3_blas_openblas_6cores.dat" t "Intel i7 5820k 3.3 GHz, OpenBLAS, 6 cores", \
#"gpu_nvidia-tesla-k40c_cuda_local.dat" t "CUDA local mem, nVIDIA Tesla K40c, 2880 PEs", \
#"gpu_amd-radeon-r9-390_cl_local.dat" t "OpenCL local mem, Radeon R9 390, 2560 PEs", \
#"gpu_nvidia-tesla-k40c_cl_local_no-data-ovhead.dat" t "OpenCL local mem w/o data overhead, nVIDIA Tesla K40c, 2880 PEs", \
#"gpu_nvidia-tesla-k40c_cl_local.dat" t "OpenCL local mem, nVIDIA Tesla K40c, 2880 PEs"

#plot\
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O2_blas_mkl_2x8cores.dat" t "Intel Xeon E5 2650 2.6 GHz, MKL BLAS, 2x8 cores",\
#"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O2_blas_openblas_2x8cores.dat" t "Intel Xeon E5 2650 2.6 GHz, OpenBLAS, 2x8 cores",\
#"corei7-5820k_3.3GHz_16MBL2_DDR43000_gcc530-O3_blas_openblas_6cores.dat" t "Intel i7 5820k 3.3 GHz, OpenBLAS, 6 cores"

plot\
"xeon-phi-knl_1.3GHz_icc17.0.2-O2_blas_mkl_64threads.dat" t "Intel Xeon Phi KNL 1.3 GHz 64 cores, MKL BLAS, 64 threads", \
"xeon-phi-knl_1.3GHz_icc17.0.2-O2_blas_mkl_128threads.dat" t "Intel Xeon Phi KNL 1.3 GHz 64 cores, MKL BLAS, 128 threads", \
"xeon-phi-knl_1.3GHz_icc17.0.2-O2_blas_mkl_192threads.dat" t "Intel Xeon Phi KNL 1.3 GHz 64 cores, MKL BLAS, 192 threads", \
"xeon-phi-knl_1.3GHz_icc17.0.2-O2_blas_mkl_256threads.dat" t "Intel Xeon Phi KNL 1.3 GHz 64 cores, MKL BLAS, 256 threads", \
"xeon-E5-2650_2.6GHz_20MBL2_icc12.1.3-O2_blas_mkl_2x8cores.dat" t "Intel Xeon E5 2650 2.6 GHz, MKL BLAS, 2x8 cores"

Measurements have been taken on the same machine used for sequential matmul.

CPU: Intel i5 650 @ 3.2 GHz (2 Cores)
L2 CACHE: 2 * 256 KB
RAM: 8GB DDR3 @ 662 MHz
Compiler: g++ 7.3.0


=================

Makefile help in compiling and executing parallel matmul

You just need to type `make` to compile all versions, execute them
and save the output to a file

=================

Mergesort can be executed via mergesort.sh
Vector sizes are specified in file `logspace`

`logspace` file has been generated with python:
	numpy.logspace(1, 8, 30)
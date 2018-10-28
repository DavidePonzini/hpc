Open the provided C++ program matmul.cpp (or the C one, matmul.c, if you prefer) and try to understand what it does, what command line parameters requires, and how it measures performance.

Then open file script.sh and customize to your needs; it is important to customize the CPU model, L2 cache size, compiler version and compilation options, so that the performance files being created by the script will record all of these things.

Launch the script and be patient, it will take some hours.  Do not run other tasks on your computer until the measurements are done.
If your computer has more than one core, you will still be able to browse the internet.

The script will create a number of new files with extension ".dat" in the directory; these files contain performance data (please examine matmul.c to understand the performance metrics being used).  To see progress during creation of these files, open another shell and type the command tail -f <name of .dat file>

When finished, draw graphs of the performance data using "gnuplot".  To this end, customize the provided configuration file matmul.gp and then invoke "gnuplot" and issue the gnuplot command ' load "matmul.gp" '.  The command will load your configuration file and execute the commands in it; and as a final result, gnuplot will create the graphs as a .png file.

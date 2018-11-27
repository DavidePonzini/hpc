set terminal png size 1920,1080 font b018015l 14
set output "out.png"
set xrange [ 0 : 9 ] reverse nowriteback
set x2range [ * : * ] reverse writeback
set yrange [ 0 : 9 ] reverse nowriteback
set y2range [ * : * ] reverse writeback
plot "out" with image

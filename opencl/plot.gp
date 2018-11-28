set terminal png size 1920,1080 font b018015l 14
set output "out.png"
set yrange [] reverse
plot "out" with image

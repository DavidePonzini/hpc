set terminal png size 1920,1080 font b018015l 14
set output ARG1."_".ARG2.".png"
set yrange [] reverse
plot ARG1 with image

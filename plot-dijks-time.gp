set term png small size 800,600
set output "plots/dijks-time.png"

set title "Dijkstra: Time x Path Len"

set xlabel "Path Len"
set ylabel "Time (s)"

set xrange [0:511]

set key default
set key box
set key samplen .2
set key ins vert
set key left top

plot "plots/dijks.data" using 1:7 title "Average-Stdev" with lines, \
     "plots/dijks.data" using 1:8 title "Average" with lines, \
     "plots/dijks.data" using 1:9 title "Average+Stdev" with lines, \
     "plots/dijks.data" using 1:10 title "Minimum" with lines, \
     "plots/dijks.data" using 1:11 title "Maximum" with lines

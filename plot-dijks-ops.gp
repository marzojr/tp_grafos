set term png small size 800,600
set output "plots/dijks-ops.png"

set title "Dijkstra: Heap Ops x Path Len"

set xlabel "Path Len"
set ylabel "Heap Ops"

set xrange [0:511]

set key default
set key box
set key samplen .2
set key ins vert
set key left top

plot "plots/dijks.data" using 1:2 title "Average-Stdev" with lines, \
     "plots/dijks.data" using 1:3 title "Average" with lines, \
     "plots/dijks.data" using 1:4 title "Average+Stdev" with lines, \
     "plots/dijks.data" using 1:5 title "Minimum" with lines, \
     "plots/dijks.data" using 1:6 title "Maximum" with lines

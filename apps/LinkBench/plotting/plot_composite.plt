set term postscript eps color enhanced dl 3
set output "_PLOTEPS_"

set pointsize 1
set format x ""
set xlabel "_XLABEL_"
set ylabel "_YLABEL_"
set encoding iso_8859_2
set nokey
set rmargin 2
set size 1,0.5

set style line 1 lt 1 lc rgb "#6495ED" lw 2
set style line 2 lt 2 lc rgb "green" lw 1
set style line 3 lt 5 lc rgb "red" lw 1
set style line 4 lt 1 lc rgb "black" lw 1

#_DISPINFO_ set label "mean: _MEAN_, median: _MED_" at 2,_LPOS_
set yrange [_YMIN_:_YMAX_]
set xrange [0:_XMAX_]

#plot "_INPUT_" u 1:2 w lines ls 1 _DISPMIN_, "_INPUT_" u 1:3 w lines ls 2 _DISPMAX_, "_INPUT_" u 1:4 w lines ls 3

plot "_INPUT_" u 1:3:4 w filledcu fs transparent solid 0.5 ls 1, "_INPUT_" u 1:3 w lines ls 4, "_INPUT_" u 1:4 w lines ls 4

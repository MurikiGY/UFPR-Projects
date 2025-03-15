#!/usr/bin/gnuplot -persist
set grid
set term png
set style line 1 lc 3 pt 7 ps 0.3
set title ARG4
set xlabel 'Entrada'
set ylabel ARG5
set output ARG6
set logscale x 2
plot ARG1 using 1:2 with linespoints  title ARG2, ARG1 using 1:3 with linespoints  title ARG3

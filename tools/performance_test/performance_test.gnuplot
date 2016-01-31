set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5
set terminal png size 400,300 enhanced font "Helvetica,10"
set xlabel "time"
set output 'cpu_data.png'
set ylabel "CPU usage in %"
plot 'cpu_data.dat' with linespoints ls 1
set output 'ram_data.png'
set ylabel "RAM usage in %"
plot 'ram_data.dat' with linespoints ls 1

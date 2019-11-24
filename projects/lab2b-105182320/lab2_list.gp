#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# lab2b_1.png
set title "List_1. Throughput vs. number of threads for mutex and spin-lock synchronized list operations"
set xlabel "Threads"
set xrange [1:25]
set logscale x 2
set ylabel "Throughput (operations/second)"
set logscale y 10
set output 'lab2b_1.png'

plot \
	"< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Mutex Lock' with linespoints lc rgb 'red', \
	"< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'Spin Lock' with linespoints lc rgb 'green'

# lab2b_2.png
set title "List_2. Mean time per mutex wait and mean time per operation for mutex-synchronized list operations"
set xlabel "Threads"
set xrange [1:25]
set logscale x 2
set ylabel "Average Time per Operation(ns)"
set logscale y 10
set output 'lab2b_2.png'

plot \
	"< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
        title 'wait-for-lock time' with linespoints lc rgb 'red', \
        "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
        title 'average operation time' with linespoints lc rgb 'green'


# lab2b_3.png
set title "List_3. Successful iterations vs. threads for each synchronization method"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Correct Iterations per Thread"
set logscale y 10
set output 'lab2b_3.png'

plot \
	"< grep 'list-id-none' lab2b_list.csv | grep '^list'" using ($2):($3) \
        title 'Unprotected with yields' with points lc rgb 'red', \
	"< grep list-id-m lab2b_list.csv | grep '^list'" using ($2):($3) \
        title 'Mutex lock with yields' with points lc rgb 'green', \
	"< grep list-id-s lab2b_list.csv | grep '^list'" using ($2):($3) \
        title 'Spin lock with yields' with points lc rgb 'blue'

# lab2b_4.png
set title "Throughput vs. number of threads for mutex synchronized partitioned lists"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (operations/second)"
set logscale y 10
set output 'lab2b_4.png'

plot \
        "< grep 'list-none-m,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '1 list' with linespoints lc rgb 'blue', \
	"< grep 'list-none-m,.*,1000,4' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '4 lists' with linespoints lc rgb 'green', \
	"< grep 'list-none-m,.*,1000,8' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '8 lists' with linespoints lc rgb 'red', \
	"< grep 'list-none-m,.*,1000,16' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '16 lists' with linespoints lc rgb 'yellow'



# lab2b_5.png
set title "Throughput vs. number of threads for spin-lock-synchronized partitioned lists"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (operations/second)"
set logscale y 10
set output 'lab2b_5.png'

plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=1' with linespoints lc rgb 'blue', \
	 "< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=4' with linespoints lc rgb 'green', \
	 "< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=8' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=16' with linespoints lc rgb 'yellow'
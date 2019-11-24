#NAME: Ziying Yu
#EMAIL: annyu@g.ucla.edu
#ID: 105182320

default:
	gcc -g -Wall -Wextra -lpthread -lprofiler lab2_list.c SortedList.c -o lab2_list

tests: default
	rm -f lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m   >> lab2b_list.csv

	./lab2_list --threads=1  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s   >> lab2b_list.csv



	-./lab2_list --threads=1  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16  --yield=id --lists=4   >> lab2b_list.csv


	./lab2_list --threads=1  --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv


	./lab2_list --threads=1  --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv



	./lab2_list --threads=1  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv


	./lab2_list --threads=1  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv

profile: default
	rm -f ./raw.gperf profile.out
	LD_PRELOAD=/u/cs/ugrad/ziying/cs111/p2b/lib/libprofiler.so
	CPUPROFILE=./raw.gperf ./lab2_list --iterations=1000 --threads=12 --sync=s
	/u/cs/ugrad/ziying/cs111/p2b/bin/pprof --text ./lab2_list ./raw.gperf > profile.out
	/u/cs/ugrad/ziying/cs111/p2b/bin/pprof --list=thread_worker ./lab2_list ./raw.gperf >> profile.out
	rm -f ./raw.gperf profile.gperf lab2_list

graphs: tests
	./lab2_list.gp

clean:
	rm -f lab2_list lab2b-105182320.tar.gz

dist: tests profile
	tar -czvf lab2b-105182320.tar.gz profile.out lab2_list.c SortedList.h SortedList.c lab2b_list.csv lab2_list.gp lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png Makefile README

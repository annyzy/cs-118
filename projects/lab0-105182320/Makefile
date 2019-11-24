# NAME: Ziying Yu
# EMAIL: annyu@g.ucla.edu
# ID: 105182320

lab0:lab0.c
	gcc -g -Wall -Wextra lab0.c -o lab0
check:
	echo "test script" > i.txt;\
	./lab0 --input=i.txt --output=o.txt;\
	if [ $$? -ne 0 ];\
	then\
		echo "Unsuccessfully copy result from input file to output file";\
	else\
		echo "Successfully copy result from input file to output file";\
	fi;\
	rm -f o.txt; 
# check invalid input
	./lab0 --input=i.txt > o.txt;\
	if [ $$? -ne 0 ];\
	then\
		echo "Invalid input file";\
	else\
		echo "Valid inputfile";\
	fi;\
	rm -f o.txt;
# check invalid output
	./lab0 --output=o.txt < i.txt ;\
	if [ $$? -ne 0 ];\
	then\
		echo "Invalid output file";\
	else\
		echo "Valid output file";\
	fi;\
	rm -f o.txt;
# check if we can open a specified input file
# redirect STDERR to a dump file
	./lab0 --input=cannotopen.txt > o.txt 2>/dev/null;\
	if [ $$? -ne 2 ];\
	then\
		echo "Successfully open input file";\
	else\
		echo "Unable to open an non-existence file";\
	fi;\
	rm -f o.txt;
# check if we can open a specified output file
# redirect STDERR to a dump file
	echo "Cannot be created" > cannotcreate.txt;\
	chmod -w cannotcreate.txt;\
	./lab0 --output=cannotcreate.txt < i.txt 2>/dev/null;\
	if [ $$? -ne 3 ];\
	then\
		echo "Successfully create output file";\
	else\
		echo "Unable to create the specified output file";\
	rm -f cannotcreate.txt;\
	fi;
# check caught and received SIGSEGV
	./lab0 --segfault --catch 2>/dev/null;\
	if [ $$? -ne 4 ];\
	then\
		echo "Uable to caught and received SIGSEGV";\
	else\
		echo "Sucessfully caught and received SIGSEGV";\
	fi;
	./lab0 --catch --segfault 2>/dev/null;\
	if [ $$? -ne 4 ];\
	then\
		echo "Uable to caught and received SIGSEGV";\
	else\
		echo "Sucessfully caught and received SIGSEGV";\
	fi;
# check unrecognized argument
	./lab0 --unrecognizedarg 2>/dev/null;\
	if [ $$? -ne 1 ];\
	then\
		echo "A valid argument";\
	else\
		echo "Unrecognized argument";\
	fi;
clean:
	rm -rf lab0 *.txt lab0-105182320.tar.gz functions.sh
dist:
	tar -czf lab0-105182320.tar.gz lab0.c Makefile README breakpoint.png backtrace.png

# NAME: Ziying Yu
# EMAIL: annyu@g.ucla.edu
# ID: 105182320

default:
	gcc -Wall -Wextra -o lab4c_tcp -lm -lmraa -g lab4c_tcp.c	
	gcc -o lab4c_tls -Wall -Wextra -lm -lmraa -g -lssl -lcrypto lab4c_tls.c
clean:
	rm -f lab4c-105182320.tar.gz lab4c_tcp lab4c_tls

dist:
	tar -czvf lab4c-105182320.tar.gz lab4c_tcp.c lab4c_tls.c README Makefile 

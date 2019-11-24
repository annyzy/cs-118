#NAME: Ziying Yu
#EMAIL: annyu@g.ucla.edu
#ID: 105182320

default:
	gcc -g -Wall -Wextra -lmcrypt lab1b-client.c -o lab1b-client
	gcc -g -Wall -Wextra -lmcrypt lab1b-server.c -o lab1b-server
client:
	gcc -g -Wall -Wextra -lmcrypt lab1b-client.c -o lab1b-client
server:
	gcc -g -Wall -Wextra -lmcrypt lab1b-server.c -o lab1b-server
clean:
	rm -rf *.txt lab1b-client lab1b-server lab1b-105182320.tar.gz
dist:
	tar -czf lab1b-105182320.tar.gz lab1b-client.c lab1b-server.c Makefile README my.key
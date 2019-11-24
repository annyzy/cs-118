//NAME: Ziying Yu
//EMAIL: annyu@g.ucla.edu
//ID: 105182320

#include <poll.h> // poll2: wait for some event on a file descriptor
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // fcntl2: manipulate file descriptor
#include <mraa.h>
#include <math.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
#include <mraa/aio.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include <stdlib.h>

#define B 4275
#define R0 100000.0


static struct option long_options[] = {
	{"period", 1, NULL,'p'},
	{"scale", 1, NULL, 's'},
	{"log", 1, NULL, 'l'},
	{"id", 1, NULL, 'i'},
	{"host", 1, NULL, 'h'},
	{0, 0, 0, 0}
};

char temp_opt = 'F';
int period = 1;
int report = 0;
int log_flag = 0;
int log_fd;

int port;
struct sockaddr_in serv_addr;
int socket_fd;

struct hostent *server;
char *host = "";
char *id = "";

mraa_aio_context tmp;

struct timeval tm;
struct tm* ts;

double cal_temp()
{
	int reading;
	reading = mraa_aio_read(tmp);
	float R = 1023.0/ ((float) reading) - 1.0;
	R = R0*R;
	float C;
	C = 1.0 / (log(R/R0)/B + 1/298.15) - 273.15;
	float F;
	F = (C*9)/5 + 32;
    if ( temp_opt == 'F')
        return F;
    else
    	return C;
}

void timestamp_report(){
	double tempreture=cal_temp();
    time_t rawtime;
    struct tm *tt;
    time(&rawtime);
    tt = localtime(&rawtime);

    dprintf(socket_fd, "%02d:%02d:%02d %.1f\n", tt->tm_hour, tt->tm_min, tt->tm_sec, tempreture);
    if(log_flag == 1 && report == 0){
        dprintf(log_fd, "%02d:%02d:%02d %.1f\n", tt->tm_hour, tt->tm_min, tt->tm_sec, tempreture);
    }
}

void shutdown_report(){
	gettimeofday(&tm, 0);
	ts = localtime(&(tm.tv_sec));

    dprintf(socket_fd, "%02d:%02d:%02d SHUTDOWN\n", ts->tm_hour, ts->tm_min, ts->tm_sec);
    if(log_flag == 1)
        dprintf(log_fd,"%02d:%02d:%02d SHUTDOWN\n", ts->tm_hour, ts->tm_min, ts->tm_sec);

    exit(0);
}

void process_cmd(char* command){
    if( !strcmp(command, "SCALE=F\n") )
    {
            temp_opt = 'F';
            if(log_flag == 1)
                dprintf(log_fd, "SCALE=F\n");
	}
    else if(!strcmp(command, "SCALE=C\n"))
    {
            temp_opt = 'C';
            if(log_flag == 1)
                dprintf(log_fd, "SCALE=C\n");
	}
    else if(!strncmp(command, "PERIOD=", 7)){
        int i = 7;
        int count = 0;
        while(command[i] != '\n'){
            i++;
            count++;
        }
        char input[count];
        int j;
        for(j = 7; j < count + 7; j++)
            input[j - 7] = command[j];
		period = atoi(input);
    	if(log_flag == 1)
        	dprintf(log_fd, "PERIOD=%d\n", period);
    }
    else if(!strcmp(command, "STOP\n"))
	{
	    report = 1;
    	if(log_flag == 1)
        	dprintf(log_fd,"STOP\n");
	}
    else if(!strcmp(command, "START\n"))
	{
    	report = 0;
    	if(log_flag==1)
        	dprintf(log_fd,"START\n");
	}
    else if(!strncmp(command, "LOG", 3))
	{
    	if(log_flag==1)
        	dprintf(log_fd, "%s\n", command);
	}
    else if(!strcmp(command, "OFF\n"))
    {
    	if(log_flag==1)
        	dprintf(log_fd, "OFF\n");
        shutdown_report();
    }
    else{
        fprintf(stderr,"input command not valid.\n");
        if(log_flag==1)
            dprintf(log_fd, command);
    }
}

int main(int argc, char* argv[]){
    int opt;

    while ((opt = getopt_long(argc, argv, "", long_options, 0)) != -1)
    {
        switch(opt)
        {
            case 'p':
                period = atoi(optarg);
                break;

            case 's':
                if(optarg[0] == 'F' || optarg[0] == 'C'){
                    temp_opt = optarg[0];
                }
                else{
                    fprintf(stderr, "Invalid scale passed.\n");
                    exit(1);
                }
                break;

            case 'l':
                log_flag = 1;
                char *file = optarg;
                log_fd = creat(file,0666);
                break;

            case 'i':
                id = optarg;
                break;
            case 'h':
                host = optarg;
                break;
            default:
                fprintf(stderr,"Invalid command options.\n");
                exit(1);
                break;
        }
    }

    //port = atoi(argv[argc - 1]);

	//https://linux.die.net/man/3/optind
	if (optind >= argc)
	{
        fprintf(stderr, "Expected port number after options.\n");
        exit(1);
    }

	port = atoi(argv[optind]);
	if(port <= 0)
	{
        fprintf(stderr, "Failed to provide port number.\n");
	}

	if(strlen(id) != 9)
    {
        fprintf(stderr, "Wrong id.\n");
        exit(1);
    }
    if(strlen(host) == 0)
    {
        fprintf(stderr, "Wrong host.\n");
        exit(1);
    }

    close(STDIN_FILENO);

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        fprintf(stderr,"Fail to initialize.\n");
        exit(1);
    }
    server = gethostbyname(host);
    if(server == NULL){
        fprintf(stderr, "Fail to host.\n");
        exit(1);
    }
	memset((void *) &serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;

    memcpy((char*)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    if(connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0){
        fprintf(stderr, "Fail to connect.\n");
        exit(1);
    }


    dprintf(socket_fd, "ID=%s\n", id);
    if(log_flag==1)
        dprintf(log_fd, "ID=%s\n", id);

    tmp = mraa_aio_init(1);
    if(tmp == NULL){
        fprintf(stderr, "Fail to initialize.\n");
        mraa_deinit();
        exit(1);
    }

    struct pollfd poll_fd[1];
    poll_fd[0].fd = socket_fd;
    poll_fd[0].events = POLLHUP | POLLERR | POLLIN;

    while(1){
        if(report == 0)
            timestamp_report();

        time_t s;
        time_t f;
        time(&s);
        time(&f);
        while(difftime(f, s) < period){
            if(poll(poll_fd, 1, 0) == -1){
                fprintf(stderr, "Fail to poll.\n");
                exit(2);
            }

            if(poll_fd[0].revents && POLLIN){
                char cmd[100];
                memset(cmd, 0, 100);
                if(read(socket_fd, cmd, 256) < 0){
                    fprintf(stderr, "Fail to read.\n");
                    exit(2);
                }
                process_cmd(cmd);
            }
            time(&f);
        }
    }

    mraa_aio_close(tmp);
    close(log_fd);

    exit(0);
}

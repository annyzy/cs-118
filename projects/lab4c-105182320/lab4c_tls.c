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

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <errno.h>
#include <openssl/err.h>

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

SSL* sslClient;
SSL_CTX *newContext;

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

void ssl_close()
{
    SSL_shutdown(sslClient);
    SSL_free(sslClient);
}

void timestamp_report()
{
	double tempreture=cal_temp();
    char buffer[100];
    time_t rawtime;
    struct tm *tt;
    time(&rawtime);
    tt = localtime(&rawtime);

    sprintf(buffer, "%02d:%02d:%02d %.1f\n", tt->tm_hour, tt->tm_min, tt->tm_sec, tempreture);
	if(SSL_write(sslClient,buffer,strlen(buffer)) < 0)
	{
        fprintf(stderr, "Filed to write ssl.\n");
        exit(2);
	}
    if(log_flag == 1 && report == 0){
        dprintf(log_fd, "%02d:%02d:%02d %.1f\n", tt->tm_hour, tt->tm_min, tt->tm_sec, tempreture);
    }
}

void shutdown_report(){
	gettimeofday(&tm, 0);
	ts = localtime(&(tm.tv_sec));

    char buffer[100];
    sprintf(buffer, "%02d:%02d:%02d SHUTDOWN\n", ts->tm_hour, ts->tm_min, ts->tm_sec);
	if(SSL_write(sslClient,buffer,strlen(buffer)) < 0)
	{
        fprintf(stderr, "Filed to write ssl.\n");
        exit(2);
	}
    if(log_flag == 1)
        dprintf(log_fd,"%02d:%02d:%02d SHUTDOWN\n", ts->tm_hour, ts->tm_min, ts->tm_sec);
    exit(0);
}

void process_cmd(char* command){
    if( ! strcmp(command, "SCALE=F\n") )
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
    else if(!strncmp(command, "PERIOD=", 7) ){
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
    else if( !strcmp(command, "STOP\n") )
	{
    	report = 1;
    	if(log_flag == 1)
        	dprintf(log_fd,"STOP\n");
	}
    else if(!strcmp(command, "START\n") )
	{
    	report = 0;
    	if(log_flag==1)
        	dprintf(log_fd,"START\n");
	}
    else if( !strncmp(command, "LOG", 3))
	{
    	if(log_flag==1)
        	dprintf(log_fd, "%s\n", command);
	}
    else if( !strcmp(command, "OFF\n") )
    {
    	if(log_flag==1)
        	dprintf(log_fd, "OFF\n");
        shutdown_report();
    }
    else{
        fprintf(stderr,"Bad command line.\n");
        if(log_flag)
            dprintf(log_fd, command);
    }
}

void tls()
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        fprintf(stderr,"Fail to initilize socketFD.\n");
        exit(2);
    }
    server = gethostbyname(host);
    if(server == NULL){
        fprintf(stderr, "Fail to get host.\n");
        exit(2);
    }

	memset((void *) &serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;

    memcpy((char*)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    if(connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0){
        fprintf(stderr, "Fail to connect.\n");
        exit(2);
    }

    SSL_library_init();
    SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

    newContext = SSL_CTX_new(TLSv1_client_method());
    if(newContext == NULL){
        fprintf(stderr,"Error on TLSv1_client_method() .\n");
        exit(2);
    }
    sslClient = SSL_new(newContext);
    if(sslClient == NULL)
    {
    	fprintf(stderr,"Fail to create sslClient.\n");
		exit(2);
    }
    if(SSL_set_fd(sslClient,socket_fd) < 0)
    {
        fprintf(stderr,"Fail to set sslClient .\n");
        exit(2);
    }
    if(SSL_connect(sslClient)!=1)
    {
        fprintf(stderr, "Faile to connect sslClient.\n");
        exit(2);
    }
}

int main(int argc, char* argv[]){
    int i;
	int scale_len = 0;
    while ((i = getopt_long(argc, argv, "p:s:l:i:h", long_options, 0)) != -1)
    {
        switch(i)
        {
            case 'p':
                period = atoi(optarg);
                break;

            case 's':
				scale_len = strlen(optarg);
				if (scale_len!= 1)
				{
					fprintf(stderr, "Incorrect length for --scale option.\n");
					exit(1);
				}

				if(optarg[0] != 'C' && optarg[0] !='F')
				{
					fprintf(stderr, "Invalid option for --scale.\n");
					exit(1);
				}
				temp_opt = optarg[0];
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
                fprintf(stderr,"Bad command line usage.\n");
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

	tls();
    char buffer[100];
    sprintf(buffer, "ID=%s\n", id);
	if(SSL_write(sslClient,buffer,strlen(buffer)) < 0)
	{
        fprintf(stderr, "Fail to write sslClient .\n");
        exit(2);
	}
    if(log_flag==1)
        dprintf(log_fd, "ID=%s\n", id);

    tmp = mraa_aio_init(1);
    if(tmp == NULL){
        fprintf(stderr, "Fail to initialize sensor.\n");
        mraa_deinit();
        exit(2);
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
                char cmd[256];
                memset(cmd, 0, 256);
                if( SSL_read(sslClient, cmd, 256) <= 0){
                    fprintf(stderr, "Fail to read from sslClient.\n");
                    exit(2);
                }
                process_cmd(cmd);
            }
            time(&f);
        }
    }

    mraa_aio_close(tmp);
    ssl_close();
	close(log_fd);
	exit(0);
}

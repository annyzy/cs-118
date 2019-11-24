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
#include <time.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <mraa/aio.h>

#define B 4275
#define R0 100000.0

mraa_aio_context temp;
mraa_gpio_context button;

//struct tm *localtime;

int period_opt = 1;
char scale_opt = 'F'; //default
FILE* log_file = 0;

int log_flag = 0;

struct timeval ts;
struct tm * tm;

int report = 1;
time_t next = 0;
//int period = 1;

static struct option long_options[] = {
	{"period", 1, NULL,'p'},
	{"scale", 1, NULL, 's'},
	{"log", 1, NULL, 'l'},
	{0, 0, 0, 0}
};

float temper_reading()
{
	int reading = mraa_aio_read(temp);
	float R = 1023.0/ ((float) reading) - 1.0;
	R = R0*R;
	float C = 1.0 / (log(R/R0)/B + 1/298.15) - 273.15;
	float F = (C*9)/5 + 32;
    if ( scale_opt == 'F')
        return F;
    else
        return C;
}

void printo(char *str, int std) 
{
    if (std == 1) {
        fprintf(stdout, "%s\n", str);
    }

    if (log_file != 0) {
        fprintf(log_file, "%s\n", str);
        fflush(log_file);
    }
}

void shutdown_report()
{
    //gettimeofday(&ts,0);
    tm = localtime( &(ts.tv_sec));
    char buf[256];
    sprintf(buf, "%02d:%02d:%02d SHUTDOWN\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    printo(buf, 1);
    //if(log_flag)
        //fputs(buf, log_flile);
    exit(0);
}

void timestamp_report()
{
    if ( gettimeofday(&ts,0)<0 )
    {
        fprintf(stderr, "Fail to get the time.\n");
        exit(1);
    }
    
    if(report && ts.tv_sec >= next)
    {
        float temperature;
        temperature = temper_reading();
        int integer = temperature * 10;
        tm = localtime( &ts.tv_sec);
        char buf[256];
        sprintf(buf, "%02d:%02d:%02d %d.%1d\n", tm->tm_hour,
                tm->tm_min, tm->tm_sec, integer/10, integer%10);
        printo(buf,1);
        next= ts.tv_sec + period_opt;
    }
}

void process_stdin(char *in) {
    int EOL = strlen(in);
    in[EOL-1] = '\0';
    while(*in == ' ' || *in == '\t') {
        in++;
    }
    char *per = strstr(in, "PERIOD=");
    char *log = strstr(in, "LOG");
    
    if(strcmp(in, "SCALE=F") == 0) {
        printo(in, 0);
        scale_opt = 'F';
    } else if(strcmp(in, "SCALE=C") == 0) {
        printo(in, 0);
        scale_opt = 'C';
    } else if(strcmp(in, "STOP") == 0) {
        printo(in, 0);
        report = 0;
    } else if(strcmp(in, "START") == 0) {
        printo(in, 0);
        report = 1;
    } else if(strcmp(in, "OFF") == 0) {
        printo(in, 0);
        shutdown_report();
    } else if(per == in) {
        char *j = in;
        j += 7;
        if(*j != 0) {
            int p = atoi(j);
            while(*j != 0) {
                if (!isdigit(*j)) {
                    return;
                }
                j++;
            }
            period_opt = p;
        }
        printo(in, 0);
    } else if (log == in) {
        printo(in, 0);
    }
}

int main(int argc, char * argv[])
{
	int i = 0;
	int scale_len = 0;
	
	while( (i= getopt_long(argc, argv, "p:s:l", long_options, NULL)) != -1)
    {
    	switch(i)
    	{
    		case 'p':
				period_opt = atoi(optarg);
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
				
				scale_opt = optarg[0];
				break;
			case 'l':
				log_file = fopen(optarg, "w+");
				if ( log_file == NULL)
				{
					fprintf(stderr, "Invalid log file.\n");
					exit(1);
				}
                log_flag = 1;
				break;
			default:
				fprintf(stderr, "Bad command-line parameters.\n");
				exit(1);
				break;
		}
    }
	
	//initialize sensors;
	
	temp = mraa_aio_init(1);
	if ( temp == NULL)
	{
		fprintf(stderr, "Failed to initialize temp sensor t0 .\n");
        mraa_deinit();
		exit(1);
	}
	
	button = mraa_gpio_init(60);
	if ( button == NULL)
	{
		fprintf(stderr, "Failed to initialize button.\n");
        mraa_deinit();
		exit(1);
	}
	
	if (mraa_gpio_dir(button, MRAA_GPIO_IN) != MRAA_SUCCESS)
        {
		fprintf(stderr,"Failed to set the direction of button.\n");
                exit(1);
        }

    /*
	if (mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &off_interrupt_handler, NULL) != MRAA_SUCCESS)
		report_error_and_exit("Failed to link button interrupt to handler");
    */
    struct pollfd p_fd;
    p_fd.fd = STDIN_FILENO;
    p_fd.events=POLLIN;
    
    
    char* buf;
    buf = (char*)malloc(1024* sizeof(char));
    if(buf == NULL){
        fprintf(stderr,"Fail to allocate memory for buf.\n");
        exit(1);
    }
    
    while(1){
        timestamp_report();
        int ret = poll(&p_fd,1,0);
        if(ret<0){
            fprintf(stderr,"Fail to poll.\n");
            exit(1);
        }
        if(p_fd.revents & POLLIN)
        {
            fgets(buf,1024,stdin);
            process_stdin(buf);
        }
        if(mraa_gpio_read(button)){
            shutdown_report();
        }
    }
    
    mraa_aio_close(temp);
    mraa_gpio_close(button);
    return 0;
    
	
}

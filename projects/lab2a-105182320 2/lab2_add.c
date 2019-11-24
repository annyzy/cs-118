// NAME: Ziying Yu
// EMAIL: annyu@g.ucla.edu
// ID: 105182320

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sched.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

int opt_yield;
char opt_sync;
pthread_mutex_t mutex;
long lock = 0;

long long counter;
long threads;
long iterations;

void add_none(long long *pointer, long long value)
{
	long long sum;
	sum = *pointer + value;
	
	if(opt_yield)
		sched_yield();
	*pointer = sum;
}

void add_mutex(long long *pointer, long long value)
{
	pthread_mutex_lock(&mutex);
	long long sum;
	
	sum = *pointer + value;
	if(opt_yield)
		sched_yield();
	*pointer = sum;
	
	pthread_mutex_unlock(&mutex);
}

/*https://attractivechaos.wordpress.com/2011/10/06/multi-threaded-programming-efficiency-of-locking/*/
void add_spinlock(long long *pointer, long long value)
{
	while(__sync_lock_test_and_set(&lock,1)) while (lock);
	long long sum;
	
	sum = *pointer + value;
	if(opt_yield)
		sched_yield();
	*pointer = sum;
	
	__sync_lock_release(&lock);
}

void add_compareswap(long long *pointer, long long value)
{
	long long prev, sum;
	do
	{
		prev = *pointer;
		sum = prev + value;
		if(opt_yield)
			sched_yield();
	}while(__sync_val_compare_and_swap(pointer, prev, sum) != prev);
}

void *thread_worker(void *arg)
{
	//unsigned long iter = *((unsigned long*)arg);
	long i;
	
	for( i=0; i<iterations; i++)
	{
		switch(opt_sync)
		{
			case 'm':
				add_mutex(&counter,1);
				add_mutex(&counter,-1);
				break;
			case 's':
				add_spinlock(&counter,1);
				add_spinlock(&counter,-1);
				break;
			case 'c':
				add_compareswap(&counter,1);
				add_compareswap(&counter,-1);
				break;
			default:
				add_none(&counter,1);
				add_none(&counter,-1);
		}
	}
	
	return arg;
}

struct option args[] = {
    {"threads", 1, NULL, 't'},
    {"iterations", 1, NULL, 'i'},
    {"yield", 0, NULL, 'y'},
    {"sync", 1, NULL, 's'},
    {0, 0, 0, 0},
};

static inline unsigned long get_nanosec_from_timespec(struct timespec *spec)
{
	unsigned long ret = spec->tv_sec;
	ret = ret * 1000000000 + spec->tv_nsec;
	return ret;
}

int main(int argc, char * argv[])
{
	ssize_t ret;
	
	//default
	threads = 1;
	iterations = 1;
	
	//initalize
	counter = 0;
	opt_sync = 0;
	opt_yield = 0;
	
	while( (ret = getopt_long(argc, argv, "", args, NULL)) != -1)
    {
    	switch(ret)
    	{
    		case 't':
    			threads = atoi(optarg);
    			break;
			case 'i':
				iterations = atoi(optarg);
				break;
			case 'y':
				opt_yield = 1;
				break;
			case 's':
			//Todo:
				opt_sync = optarg[0];
				break;
			default:
				fprintf(stderr, "%s: Bad command-line parameters", strerror(errno));
				exit(1);
		}
    }
	
	if(opt_sync == 'm')
	{
		if( pthread_mutex_init(&mutex, NULL) != 0 )
		{
			fprintf(stderr, "%s: Cannnot create mutex", strerror(errno));
			exit(1);
		}
	}
	
    struct timespec begin, end;
    long diff, operations,average_time_per_operation;
    diff = 0;
	
    if (clock_gettime(CLOCK_MONOTONIC, &begin) < 0)
    {
		fprintf(stderr, "%s: Cannot get the clock begin time", strerror(errno));
		exit(1);
	}
	
	pthread_t *id = malloc(threads * sizeof(pthread_t));
	if( id == NULL)
	{
		fprintf(stderr, "%s: Cannot allocate memory for id", strerror(errno));
		exit(1);
	}
	
	int i;
	for(i = 0; i<threads; i++)
	{
		if( pthread_create(&id[i], NULL, &thread_worker, NULL) != 0 )
		{
			fprintf(stderr, "%s: Cannot allocate memory for id", strerror(errno));
			exit(1);
		}
	}
	
	for(i = 0; i<threads; i++)
	{
		if ( pthread_join(id[i], NULL) != 0 )
		{
			fprintf(stderr, "%s: Cannot join the thread ids", strerror(errno));
			exit(1);
		}
	}
	
    if (clock_gettime(CLOCK_MONOTONIC, &end) < 0)
    {
		fprintf(stderr, "%s: Cannot get the clock begin time", strerror(errno));
		exit(1);
	}
	
	diff = get_nanosec_from_timespec(&end)-get_nanosec_from_timespec(&begin);
	operations = threads * iterations *2;
	average_time_per_operation = diff / operations;
	
	if(opt_yield ==1)
	{
		if(opt_sync == 0)
			fprintf(stdout, "add-yield-none,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, diff, average_time_per_operation,counter);
		if(opt_sync == 'm')
			fprintf(stdout, "add-yield-m,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, diff, average_time_per_operation,counter);
		if(opt_sync == 's')
			fprintf(stdout, "add-yield-s,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, diff, average_time_per_operation,counter);
		if(opt_sync == 'c')
			fprintf(stdout, "add-yield-c,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, diff, average_time_per_operation,counter);
	}
	
	if(opt_yield == 0)
	{
		if(opt_sync == 0)
			fprintf(stdout, "add-none,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, diff, average_time_per_operation,counter);
		if(opt_sync == 'm')
			fprintf(stdout, "add-m,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, diff, average_time_per_operation,counter);
		if(opt_sync == 's')
			fprintf(stdout, "add-s,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, diff, average_time_per_operation,counter);
		if(opt_sync == 'c')
			fprintf(stdout, "add-c,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, diff, average_time_per_operation,counter);
	}
	
	if(opt_sync == 'm')
	{
		if( pthread_mutex_destroy(&mutex) != 0 )
		{
			fprintf(stderr, "%s: Cannnot destroy mutex", strerror(errno));
			exit(1);
		}
	}
	
	free(id);
	exit(0);
}





// NAME: Ziying Yu
// EMAIL: annyu@g.ucla.edu
// ID: 105182320

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sched.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#include "SortedList.h"

//#define KEY_LEN         2


int iterations;
int threads;
//int iterations = 1；
//int threads = 1；

long lock = 0;

pthread_mutex_t mutex;
SortedList_t *listhead;
SortedListElement_t *pool;

int opt_yield;
int opt_sync;
//int opt_yield =0；
//int opt_sync = 0；

int num_list_element;
//Note that in some cases your program may not detect an error, but may simply experience a segmentation fault.
//Catch these, and log and return an error
void sigsegv_handler(int sig)
{
	if (sig == SIGSEGV)
	{
		fprintf(stderr, "Segmentation fault is caught.\n");
		exit(2);
    }
}

/*
each thread:
starts with a set of pre-allocated and initialized elements (--iterations=#)
inserts them all into a (single shared-by-all-threads) list
gets the list length
looks up and deletes each of the keys it had previously inserted
exits to re-join the parent thread
*/
void *thread_worker(void *arg)
{
//	if(opt_sync == 'm')
//	{
//		if( pthread_mutex_lock(&mutex)!= 0)
//		{
//			fprintf(stderr, "%s:Cannot lock by mutex.\n", strerror(errno));
//			exit(1);
//		}
//	}
//
//	if (opt_sync == 's')
//	{
//		while (__sync_lock_test_and_set(&lock, 1)) while (lock);
//	}
	int i;
	/*long thread_num = *((long*)arg);

	//long start_index = thread_num * iterations;*/

	//inserts them all into a (single shared-by-all-threads) list
	i = *((long*)arg);
	
	for ( /*i = start_index*/; i < /*start_index + iterations*/ num_list_element; i+=/*+*/threads)
	{
		if(opt_sync == 'm')
		{
			if( pthread_mutex_lock(&mutex)!= 0)
			{
				fprintf(stderr, "Cannot lock by mutex.\n");
				exit(1);
			}
		}

		if (opt_sync == 's')
		{
			while (__sync_lock_test_and_set(&lock, 1)) while (lock);
		}

		SortedList_insert(listhead, &pool[i]);

		if(opt_sync == 'm')
		{
			pthread_mutex_unlock(&mutex);
		}

		if (opt_sync == 's')
		{
			__sync_lock_release(&lock);
		}
	}

	if(opt_sync == 'm')
	{
		if( pthread_mutex_lock(&mutex)!= 0)
		{
			fprintf(stderr, "Cannot lock by mutex.\n");
			exit(1);
		}
	}

	if (opt_sync == 's')
	{
		while (__sync_lock_test_and_set(&lock, 1)) while (lock);
	}

	//gets the list length
	int length;
	length = SortedList_length(listhead);

	if(length < 0)
	{
			fprintf(stderr, "Cannot get the correct length, list is corrupted.\n");
			exit(2);
	}

	if(opt_sync == 'm')
	{
		pthread_mutex_unlock(&mutex);
	}

	if (opt_sync == 's')
	{
		__sync_lock_release(&lock);
	}

	SortedListElement_t* ptr;
	//looks up and deletes each of the keys it had previously inserted
	for ( /*i = start_index*/; i < /*start_index + iterations*/ num_list_element; i+=/*+*/threads)
	{
		if(opt_sync == 'm')
		{
			if( pthread_mutex_lock(&mutex)!= 0)
			{
				fprintf(stderr, "Cannot lock by mutex.\n");
				exit(1);
			}
		}

		if (opt_sync == 's')
		{
			while (__sync_lock_test_and_set(&lock, 1)) while (lock);
		}

		ptr = SortedList_lookup(listhead, pool[i].key);

		if (ptr == NULL || SortedList_delete(ptr))
		{
			fprintf(stderr, "Cannot delete the target, list is corrupted.\n");
			exit(2);
		}
		if(opt_sync == 'm')
		{
			pthread_mutex_unlock(&mutex);
		}

		if (opt_sync == 's')
		{
			__sync_lock_release(&lock);
		}
	}
	
//	if(opt_sync == 'm')
//	{
//		pthread_mutex_unlock(&mutex);
//	}
//
//	if (opt_sync == 's')
//	{
//		__sync_lock_release(&lock);
//	}

	return NULL;
}

//static inline unsigned long get_nanosec_from_timespec(struct timespec *spec)
//{
//	unsigned long ret = spec->tv_sec;
//	ret = ret * 1000000000 + spec->tv_nsec;
//	return ret;
//}
//
struct option long_options[] = {
    {"threads", 1, NULL, 't'},
    {"iterations", 1, NULL, 'i'},
    {"yield", 1, NULL, 'y'},
    {"sync", 1, NULL, 's'},
    {0, 0, 0, 0},
};

int main(int argc, char * argv[])
{
	int ret = 0;

	//default
	threads = 1;
	iterations = 1;

	//initalize
	opt_sync = 0;
	opt_yield = 0;

	int i = 0;
	int len;
//	int j = 0;
	//char yield_
	while( (ret = getopt_long(argc, argv, "", long_options, NULL)) != -1)
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
				len = strlen(optarg);
				if (len > 3)
				{
					fprintf(stderr, "Invalid command-line parameters for --yield.\n ");
					exit(1);
				}

				//opt_yield = optarg;

				for(i = 0; i < len; i++)
				{
					if(optarg[i] =='i' )
						opt_yield |= INSERT_YIELD;
					if(optarg[i] =='d' )
						opt_yield |= DELETE_YIELD;
					if(optarg[i] =='l' )
						opt_yield |= LOOKUP_YIELD;
				}
				break;
			case 's':
				opt_sync = optarg[0];
				break;
			default:
				fprintf(stderr, "Bad command-line parameters.\n");
				exit(1);
		}
    }

	signal(SIGSEGV,sigsegv_handler);

	if(opt_sync == 'm')
	{
		if( pthread_mutex_init(&mutex, NULL) != 0 )
		{
			fprintf(stderr, "Cannnot create mutex.\n");
			exit(1);
		}
	}

    //initializes an empty list
	listhead = malloc(sizeof(SortedList_t));
	listhead->next = listhead;
	listhead->prev = listhead;
	listhead->key = NULL;

    //creates and initializes (with random keys) the required number (threads x iterations) of list elements
    num_list_element = threads * iterations;
    pool  = malloc( num_list_element * sizeof(SortedListElement_t) );

	if( pool ==NULL)
	{
		fprintf(stderr, "Cannot allocate memory for pool.\n");
		exit(1);
	}

	srand(time(NULL));
	for (i = 0; i < num_list_element; i++)
	{
		int random_char = 'a' + rand() % 26;
		char* random_key = malloc(2 * sizeof(char));
		if (random_key == NULL) {
			fprintf(stderr, "Could not allocate memory for keys.\n");
			exit(1);
		}
		random_key[0] = random_char;
		random_key[1] = '\0';
		
		pool[i].key = random_key;
		
	}

//	int* thread_id = malloc(threads * sizeof(int));
//	int val;
//	for (i = 0, val = 0; i < threads; i++, val += iterations){
//		thread_id[i] = val;
//	}
	
	struct timespec begin, end;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	//starts the specified number of threads
	pthread_t *id = (pthread_t*) malloc(threads * sizeof(pthread_t));
	int* pid = malloc(threads * sizeof(int));
//	if(pid == NULL)
//	{
//		fprintf(stderr, "%s: Cannot allocate memory for pid", strerror(errno));
//		free(pool);
//		exit(1);
//	}
	
	if(id == NULL)
	{
		fprintf(stderr, "Cannot allocate memory for id.\n");
		free(pool);
		exit(1);
	}

//    {
//		fprintf(stderr, "%s: Cannot get the clock begin time", strerror(errno));
//		exit(1);
//	}

//	for(i = 0, begin_po = 0; i<threads; i++, begin_pos += iterations)
	for(i = 0; i<threads; i++)
	{
		pid[i] = i;
		if( pthread_create(id + i , NULL, thread_worker, &pid[i] ) != 0)
		{
			fprintf(stderr, "Cannot allocate memory for id.\n");
//			free(id);
//			free(begin_pos);
//			free(pool);
			exit(1);
		}
	}

	for(i = 0; i<threads; i++)
	{
		if ( pthread_join( *(id+i), NULL) !=0 )
		{
			fprintf(stderr, "Cannot join the thread ids.\n");
//			free(id);
//			free(begin_pos);
//			free(pool);
			exit(1);
		}
	}

	//notes the (high resolution) ending time for the run
    if ( clock_gettime(CLOCK_MONOTONIC, &end) != 0 )
    {
		fprintf(stderr, "Cannot get the clock begin time.\n");
		exit(1);
	}
	
//    {
//		fprintf(stderr, "%s: Cannot get the clock begin time", strerror(errno));
//		exit(1);
//	}

//	long diff,operations,average_time_per_operation;
//	diff = get_nanosec_from_timespec(&end)-get_nanosec_from_timespec(&begin);
//	operations = threads * iterations * 3;
//	average_time_per_operation = diff / operations;

	//checks the length of the list to confirm that it is zero
//	if ( SortedList_length(listhead) != 0 )
//	{
//		fprintf(stderr, "%s: final length of the list is not 0", strerror(errno));
//		exit(2);
//	}

	//prints to stdout a comma-separated-value (CSV) record
	fprintf(stdout,"list-");

	/*
	the name of the test, which is of the form: list-yieldopts-syncopts: where
	yieldopts = {none, i,d,l,id,il,dl,idl}
	syncopts = {none,s,m}
	*/

	switch(opt_yield)
	{
//		case 0:
//			fprintf(stdout, "none,");
//			break;
		case 1:
			fprintf(stdout, "i");
			break;
		case 2:
			fprintf(stdout, "d");
			break;
		case 3:
			fprintf(stdout, "l");
			break;
		case 4:
			fprintf(stdout, "id");
			break;
		case 5:
			fprintf(stdout, "il");
			break;
		case 6:
			fprintf(stdout, "dl");
			break;
		case 7:
			fprintf(stdout, "idl");
			break;
		default:
			fprintf(stdout, "none");
			break;
	}

	switch(opt_sync)
	{
//		case 0:
//			fprintf(stdout, "-none,");
//			break;
		case 's':
			fprintf(stdout, "-s,");
			break;
		case 'm':
			fprintf(stdout, "-m,");
			break;
		default:
			fprintf(stdout, "-none,");
			break;
	}

//	long long diff;
//	int operations;
	int operations = threads * iterations * 3;
//	diff = get_nanosec_from_timespec(&end)-get_nanosec_from_timespec(&begin);
	long long diff = 1000000000 * (end.tv_sec - begin.tv_sec) + end.tv_nsec - begin.tv_nsec;
	int average_time_per_operation = diff / operations;

	int num_lists = 1;
	fprintf(stdout, "%d,%d,%d,%d,%lld,%d\n", threads, iterations, num_lists, operations, diff, average_time_per_operation);

	if(opt_sync == 'm')
	{
		if( pthread_mutex_destroy(&mutex) != 0 )
		{
			fprintf(stderr, "Cannnot destroy mutex.\n");
			exit(1);
		}
	}
	
	free(id);
	free(pool);
	
//		//checks the length of the list to confirm that it is zero
//	if ( SortedList_length(listhead) != 0 )
//	{
//		fprintf(stderr, "%s: final length of the list is not 0", strerror(errno));
//		exit(2);
//	}
//
	exit(0);

}


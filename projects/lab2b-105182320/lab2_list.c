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

int iterations =1;
int threads=1;

int opt_yield=0;
int opt_sync=0;
int num_lists = 1;

//long lock = 0;
pthread_mutex_t *mutex;
int *lock;

SortedList_t *listhead;
SortedListElement_t *pool;
int num_list_element;

int *list;
//long long lock_time;//
long long lock_time = 0;

struct option long_options[] = {
    {"threads", 1, NULL, 't'},
    {"iterations", 1, NULL, 'i'},
    {"yield", 1, NULL, 'y'},
    {"sync", 1, NULL, 's'},
    {"lists",1, NULL, 'l'},
    {0, 0, 0, 0},
};

void sigsegv_handler(int sig)
{
	if (sig == SIGSEGV)
	{
		fprintf(stderr, "Segmentation fault is caught.\n");
		exit(2);
    }
}

int hash_key(const char *key)
{
	return (key[0] % num_lists);
}
//
//void distribute_pool()
//{
//	list = malloc(num_list_element*sizeof(int));
//	if ( list == NULL)
//	{
//		fprintf(stderr, "Cannot allocate memory for list.\n");
//		exit(1);
//	}
//	int i;
//	for (i =0; i < num_list_element; i++)
//		list[i] = hash_key(pool[i].key);
//}

//void *thread_worker(void *arg)
//{
//	int i;
//	i = *((long*)arg);
//
//	struct timespec lock_begin, lock_end;
//
//	for (; i <  num_list_element; i+=threads)
//	{
//		if(opt_sync == 'm')
//		{
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
//			{
//				fprintf(stderr, "error starting mutex timing");
//				exit(1);
//			}
//
//			if (pthread_mutex_lock(&mutex[list[i]]) !=0 )
//			{
//				fprintf(stderr, "Cannot lock by mutex.\n");
//				exit(1);
//			}
//
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
//			{
//				fprintf(stderr, "error starting mutex timing");
//				exit(1);
//			}
//
//			lock_time += 1000000000 * (lock_end.tv_sec - lock_begin.tv_sec) + lock_end.tv_nsec - lock_begin.tv_nsec;
//			SortedList_insert(&listhead[list[i]], &pool[i]);
//
//			if( pthread_mutex_unlock(&mutex[list[i]])!= 0)
//			{
//				fprintf(stderr, "Cannot unlock by mutex.\n");
//				exit(1);
//			}
//		}
//
//		else if (opt_sync == 's')
//		{
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
//			{
//				fprintf(stderr, "Cannot start mutex timing");
//				exit(1);
//			}
//
//			while (__sync_lock_test_and_set(&lock[list[i]], 1));
//
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
//			{
//				fprintf(stderr, "Cannot end mutex timing");
//				exit(1);
//			}
//
//			lock_time += 1000000000 * (lock_end.tv_sec - lock_begin.tv_sec) + lock_end.tv_nsec - lock_begin.tv_nsec;
//			SortedList_insert(&listhead[list[i]], &pool[i]);
//
//			__sync_lock_release(&lock[list[i]]);
//
//		}
//		else
//			SortedList_insert(&listhead[list[i]], &pool[i]);
//	}
//
//	//gets the list length
//	int lengths;
//	lengths = 0;
//	int len;
//
//	for ( i = 0; i < num_lists; i++)
//	{
//		if(opt_sync == 'm')
//		{
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
//			{
//				fprintf(stderr, "Cannot not start mutex timing.\n");
//				exit(1);
//			}
//
//			if( pthread_mutex_lock(&mutex[i])!= 0)
//			{
//				fprintf(stderr, "Cannot lock by mutex.\n");
//				exit(1);
//			}
//
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
//			{
//				fprintf(stderr, "Error on ending mutex timing.\n");
//				exit(1);
//			}
//
//			lock_time += 1000000000 * (lock_end.tv_sec - lock_begin.tv_sec) + lock_end.tv_nsec - lock_begin.tv_nsec;
//			len = SortedList_length(&listhead[i]);
//			if ( len < 0)
//			{
//				fprintf(stderr, "Cannot get the correct length.\n");
//				exit(1);
//			}
//			lengths += len;
//
//			if( pthread_mutex_unlock(&mutex[i])!= 0)
//			{
//				fprintf(stderr, "Cannot unlock by mutex.\n");
//				exit(1);
//			}
//		}
//
//		else if (opt_sync == 's')
//		{
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
//			{
//				fprintf(stderr, "Cannot start mutex timing.\n");
//				exit(1);
//			}
//
//			while (__sync_lock_test_and_set(&lock[i], 1)) while (lock[i]);
//
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
//			{
//				fprintf(stderr, "Cannot end mutex timing.\n");
//				exit(1);
//			}
//
//			lock_time += 1000000000 * (lock_end.tv_sec - lock_begin.tv_sec) + lock_end.tv_nsec - lock_begin.tv_nsec;
//			len = SortedList_length(&listhead[i]);
//			if ( len < 0)
//			{
//				fprintf(stderr, "Cannot get the correct length.\n");
//				exit(1);
//			}
//			lengths += len;
//
//			__sync_lock_release(&lock[list[i]]);
//		}
//
//		else
//		{
//			len = SortedList_length(&listhead[i]);
//			if ( len < 0)
//			{
//				fprintf(stderr, "Cannot get the correct length.\n");
//				exit(1);
//			}
//			lengths += len;
//		}
//	}
//
//	SortedListElement_t* ptr;
//	i = *(int*)arg;
//	//looks up and deletes each of the keys it had previously inserted
//	for ( ; i <  num_list_element; i+= threads)
//	{
//		if(opt_sync == 'm')
//		{
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
//			{
//				fprintf(stderr, "error starting mutex timing");
//				exit(1);
//			}
//
//			if( pthread_mutex_lock(&mutex[list[i]])!= 0)
//			{
//				fprintf(stderr, "Cannot lock by mutex.\n");
//				exit(1);
//			}
//
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
//			{
//				fprintf(stderr, "error starting mutex timing");
//				exit(1);
//			}
//
//			lock_time += 1000000000 * (lock_end.tv_sec - lock_begin.tv_sec) + lock_end.tv_nsec - lock_begin.tv_nsec;
//			ptr = SortedList_lookup(&listhead[list[i]], pool[i].key);
//
//			if (ptr == NULL)
//			{
//				fprintf(stderr, "Cannot delete the target, list is corrupted.\n");
//				exit(2);
//			}
//
//			if( SortedList_delete(ptr))
//			{
//				fprintf(stderr, "Cannot delete the target, list is corrupted.\n");
//				exit(2);
//			}
//
//			if( pthread_mutex_unlock(&mutex[list[i]])!= 0)
//			{
//				fprintf(stderr, "Cannot unlock by mutex.\n");
//				exit(1);
//			}
//
//		}
//
//		else if (opt_sync == 's')
//		{
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
//			{
//				fprintf(stderr, "error starting mutex timing");
//				exit(1);
//			}
//
//			while (__sync_lock_test_and_set(&lock[list[i]], 1)) while (lock[list[i]]);
//
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
//			{
//				fprintf(stderr, "error starting mutex timing");
//				exit(1);
//			}
//
//			lock_time += 1000000000 * (lock_end.tv_sec - lock_begin.tv_sec) + lock_end.tv_nsec - lock_begin.tv_nsec;
//			ptr = SortedList_lookup(&listhead[list[i]], pool[i].key);
//
//			if (ptr == NULL)
//			{
//				fprintf(stderr, "Cannot delete the target, list is corrupted.\n");
//				exit(2);
//			}
//
//			if( SortedList_delete(ptr))
//			{
//				fprintf(stderr, "Cannot delete the target, list is corrupted.\n");
//				exit(2);
//			}
//
//			__sync_lock_release(&lock[list[i]]);
//		}
//
//		else
//		{
//
//			ptr = SortedList_lookup(&listhead[list[i]], pool[i].key);
//
//			if (ptr == NULL)
//			{
//				fprintf(stderr, "Cannot delete the target, list is corrupted.\n");
//				exit(2);
//			}
//
//			if( SortedList_delete(ptr))
//			{
//				fprintf(stderr, "Cannot delete the target, list is corrupted.\n");
//				exit(2);
//			}
//		}
//	}
//	return NULL;
//}


void *thread_worker(void *arg)
{
	int i = *((long*)arg);

	struct timespec lock_begin, lock_end;

	for (; i <  num_list_element; i+=threads)
	{
		if(opt_sync == 'm')
		{
			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
			{
				fprintf(stderr, "error starting mutex timing");
				exit(1);
			}
			//clock_gettime(CLOCK_MONOTONIC, &lock_begin);
			//pthread_mutex_lock(&mutex[list[i]]);
			if (pthread_mutex_lock(&mutex[list[i]]) !=0 )
			{
				fprintf(stderr, "Cannot lock by mutex.\n");
				exit(1);
			}
			//clock_gettime(CLOCK_MONOTONIC, &lock_end);
			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
			{
				fprintf(stderr, "error starting mutex timing");
				exit(1);
			}
		}

		else if (opt_sync == 's')
		{
			//clock_gettime(CLOCK_MONOTONIC, &lock_begin);
			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
			{
				fprintf(stderr, "Cannot start mutex timing");
				exit(1);
			}

			while (__sync_lock_test_and_set(&lock[list[i]], 1));

			//clock_gettime(CLOCK_MONOTONIC, &lock_end);
			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
			{
				fprintf(stderr, "Cannot end mutex timing");
				exit(1);
			}
		}
		lock_time += 1000000000 * (lock_end.tv_sec - lock_begin.tv_sec) + lock_end.tv_nsec - lock_begin.tv_nsec;
		SortedList_insert(&listhead[list[i]], &pool[i]);

		if (opt_sync == 'm')
		{
//			pthread_mutex_unlock(&mutex[list[i]]);
			if( pthread_mutex_unlock(&mutex[list[i]])!= 0)
			{
				fprintf(stderr, "Cannot unlock by mutex.\n");
				exit(1);
			}
		}

		else if (opt_sync == 's')
			__sync_lock_release(&lock[list[i]]);
	}

	//gets the list length
	int lengths;
	lengths = 0;

	for ( i = 0; i < num_lists; i++)
	{
		if(opt_sync == 'm')
		{
		
			clock_gettime(CLOCK_MONOTONIC, &lock_begin);
			pthread_mutex_lock(&mutex[list[i]]);
			clock_gettime(CLOCK_MONOTONIC, &lock_end);
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
//			{
//				fprintf(stderr, "Cannot not start mutex timing.\n");
//				exit(1);
//			}
//
//			if( pthread_mutex_lock(&mutex[i])!= 0)
//			{
//				fprintf(stderr, "Cannot lock by mutex.\n");
//				exit(1);
//			}
//
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
//			{
//				fprintf(stderr, "Error on ending mutex timing.\n");
//				exit(1);
//			}
		}
		else if (opt_sync == 's')
		{
		
			clock_gettime(CLOCK_MONOTONIC, &lock_begin);
			while (__sync_lock_test_and_set(&lock[list[i]], 1));
			clock_gettime(CLOCK_MONOTONIC, &lock_end);
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
//			{
//				fprintf(stderr, "Cannot start mutex timing.\n");
//				exit(1);
//			}
//
//			while (__sync_lock_test_and_set(&lock[i], 1)) while (lock[i]);
//
//			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
//			{
//				fprintf(stderr, "Cannot end mutex timing.\n");
//				exit(1);
//			}
		}

		lock_time += 1000000000 * (lock_end.tv_sec - lock_begin.tv_sec) + lock_end.tv_nsec - lock_begin.tv_nsec;
		int len = SortedList_length(&listhead[i]);
		lengths += len;

//		if ( len < 0)
//		{
//			fprintf(stderr, "Cannot get the correct length.\n");
//			exit(1);
//		}

		if (opt_sync == 'm')
		{
//			pthread_mutex_unlock(&mutex[list[i]]);
			if( pthread_mutex_unlock(&mutex[list[i]])!= 0)
			{
				fprintf(stderr, "Cannot unlock by mutex.\n");
				exit(1);
			}
		}

		else if (opt_sync == 's')
			__sync_lock_release(&lock[list[i]]);
	}

	i = *(int*)arg;
	SortedListElement_t* ptr;
	//looks up and deletes each of the keys it had previously inserted
	for ( ; i <  num_list_element; i+= threads)
	{
		if(opt_sync == 'm')
		{
//			clock_gettime(CLOCK_MONOTONIC, &lock_begin);
//			pthread_mutex_lock(&mutex[list[i]]);
//			clock_gettime(CLOCK_MONOTONIC, &lock_end);
			
			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
			{
				fprintf(stderr, "error starting mutex timing");
				exit(1);
			}

			if( pthread_mutex_lock(&mutex[list[i]])!= 0)
			{
				fprintf(stderr, "Cannot lock by mutex.\n");
				exit(1);
			}

			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
			{
				fprintf(stderr, "error starting mutex timing");
				exit(1);
			}
		}
		else if (opt_sync == 's')
		{
//			clock_gettime(CLOCK_MONOTONIC, &lock_begin);
//			while (__sync_lock_test_and_set(&lock[list[i]], 1));
//			clock_gettime(CLOCK_MONOTONIC, &lock_end);
			if (clock_gettime(CLOCK_MONOTONIC, &lock_begin) == -1)
			{
				fprintf(stderr, "error starting mutex timing");
				exit(1);
			}

			while (__sync_lock_test_and_set(&lock[list[i]], 1)) while (lock[list[i]]);

			if (clock_gettime(CLOCK_MONOTONIC, &lock_end) == -1)
			{
				fprintf(stderr, "error starting mutex timing");
				exit(1);
			}
		}

		lock_time += 1000000000 * (lock_end.tv_sec - lock_begin.tv_sec) + lock_end.tv_nsec - lock_begin.tv_nsec;
		ptr = SortedList_lookup(&listhead[list[i]], pool[i].key);

		if (ptr == NULL)
		{
			fprintf(stderr, "Cannot delete the target, list is corrupted.\n");
			exit(2);
		}

		if( SortedList_delete(ptr))
		{
			fprintf(stderr, "Cannot delete the target, list is corrupted.\n");
			exit(2);
		}

		if (opt_sync == 'm')
		{
			if( pthread_mutex_unlock(&mutex[list[i]])!= 0)
			{
				fprintf(stderr, "Cannot unlock by mutex.\n");
				exit(1);
			}
		}

		else if (opt_sync == 's')
		{
			__sync_lock_release(&lock[list[i]]);
		}
	}
	return NULL;
}

int main(int argc, char * argv[])
{

//	//default
//	threads = 1;
//	iterations = 1;
//
//	//initalize
//	opt_sync = 0;
//	opt_yield = 0;

	int i = 0;
	int lens;
	//char yieldopt[10] = "";
	while( (i= getopt_long(argc, argv, "t:i:y:s", long_options, NULL)) != -1)
    {
    	switch(i)
    	{
    		case 't':
    			threads = atoi(optarg);
    			break;
			case 'i':
				iterations = atoi(optarg);
				break;
			case 'y':
				lens= strlen(optarg);

				if (lens > 3)
				{
					fprintf(stderr, "Invalid command-line parameters for --yield.\n ");
					exit(1);
				}

				for(int j = 0; j < lens; j++)
				{
					if(optarg[j] =='i' )
					{
						opt_yield |= INSERT_YIELD;
						//strcat(yieldopt, "i");
					}
					else if(optarg[j] =='d' )
					{
						opt_yield |= DELETE_YIELD;
						//strcat(yieldopt, "d");
					}
					else if(optarg[j] =='l' )
					{
						opt_yield |= LOOKUP_YIELD;
						//strcat(yieldopt, "l");
					}
					else
					{
						fprintf(stderr, "Error: invalid argment for --yield.\n");
						exit(1);
					}
//
//	      				switch (optarg[j])
//						{
//							case 'i':
//								opt_yield |= INSERT_YIELD;
//								strcat(yieldopt, "i");
//		  						break;
//							case 'd':
//								opt_yield |= DELETE_YIELD;
//								strcat(yieldopt, "d");
//		  						break;
//							case 'l':
//								opt_yield |= LOOKUP_YIELD;
//								strcat(yieldopt, "l");
//		  						break;
//							default:
//		  						fprintf(stderr, "error: invalid argument for yield option.\n");
//		  						exit(1);
//						}

//					switch (optarg[j])
//					{
//						case 'i':
//							strcat(yieldopt, "i");
//							opt_yield = opt_yield & INSERT_YIELD;
//							break;
//						case 'd':
//							strcat(yieldopt, "d");
//							opt_yield = opt_yield & DELETE_YIELD;
//							break;
//						case 'l':
//							strcat(yieldopt, "l");
//							opt_yield = opt_yield & LOOKUP_YIELD;
//							break;
//						default:
//							fprintf(stderr, "error: invalid argument for yield option.\n");
//							exit(1);
//					}
				}
					break;
			case 's':
				opt_sync = optarg[0];
                if (opt_sync != 'm' && opt_sync != 's')
                {
                    fprintf(stderr, "Error: invalid argument for --sync.\n");
                    exit(1);
                }
				break;
			case 'l':
				num_lists = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Bad command-line parameters.\n");
				exit(1);
				//break;
		}
    }

	signal(SIGSEGV,sigsegv_handler);

	//initialized lock
	if(opt_sync == 'm')
	{
		mutex = malloc(num_lists * sizeof(pthread_mutex_t));
		if( mutex ==NULL)
		{
			fprintf(stderr, "Cannot allocate memory for mutex.\n");
			exit(1);
		}
		for (i = 0;i < num_lists; i++)
		{
			if( pthread_mutex_init(&mutex[i], NULL) != 0 )
			{
				fprintf(stderr, "Cannnot initialize mutex[i].\n");
				exit(1);
			}
		}
	}

	if(opt_sync == 's')
	{
		lock = malloc(num_lists * sizeof(int));
		if( lock ==NULL)
		{
			fprintf(stderr, "Cannot allocate memory for lock.\n");
			exit(1);
		}
		for (i = 0;i < num_lists; i++)
		{
			lock[i] = 0;
		}
	}

    //initializes an empty list
	listhead = malloc(sizeof(SortedList_t) * num_lists);
	if( listhead ==NULL)
	{
		fprintf(stderr, "Cannot allocate memory for lock.\n");
		exit(1);
	}
	for (i = 0;i < num_lists; i++)
	{
		listhead[i].next = &listhead[i];
		listhead[i].prev = &listhead[i];
		listhead[i].key = NULL;
	}

    //initializes (with random keys) the required number (threads x iterations) of list elements
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
		if (random_key == NULL)
		{
			fprintf(stderr, "Could not allocate memory for keys.\n");
			exit(1);
		}
		random_key[0] = random_char;
		random_key[1] = '\0';

		pool[i].key = random_key;

	}

	list = malloc(num_list_element*sizeof(int));
	if ( list == NULL)
	{
		fprintf(stderr, "Cannot allocate memory for list.\n");
		exit(1);
	}

	for (i =0; i < num_list_element; i++)
	{
		list[i] = hash_key(pool[i].key);
	}

	struct timespec begin, end;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	//starts the specified number of threads
	pthread_t *id = malloc(threads * sizeof(pthread_t));
	int* pid = malloc(threads * sizeof(int));
	if(id == NULL)
	{
		fprintf(stderr, "Cannot allocate memory for id.\n");
		free(pool);
		exit(1);
	}
	if(pid == NULL)
	{
		fprintf(stderr, "Cannot allocate memory for id.\n");
		free(pool);
		exit(1);
	}
	for(i = 0; i<threads; i++)
	{
		pid[i] = i;
		if( pthread_create(id + i , NULL, thread_worker, &pid[i] ))
		{
			fprintf(stderr, "Cannot allocate memory for id.\n");

			exit(1);
		}
	}

	for(i = 0; i<threads; i++)
	{
		if ( pthread_join( *(id+i), NULL))
		{
			fprintf(stderr, "Cannot join the thread ids.\n");

			exit(1);
		}
	}

	//notes the (high resolution) ending time for the run
    if ( clock_gettime(CLOCK_MONOTONIC, &end) != 0 )
    {
		fprintf(stderr, "Cannot get the clock begin time.\n");
		exit(1);
	}

  	fprintf(stdout, "list");

	switch(opt_yield) {
	    case 0:
	        fprintf(stdout, "-none");
	        break;
	    case 1:
	        fprintf(stdout, "-i");
	        break;
	    case 2:
	        fprintf(stdout, "-d");
	        break;
	    case 3:
	        fprintf(stdout, "-id");
	        break;
	    case 4:
	        fprintf(stdout, "-l");
	        break;
	    case 5:
	        fprintf(stdout, "-il");
	        break;
	    case 6:
	        fprintf(stdout, "-dl");
	        break;
	    case 7:
	        fprintf(stdout, "-idl");
	        break;
	    default:
	        break;
	}

	switch(opt_sync) {
	    case 0:
	        fprintf(stdout, "-none");
	        break;
	    case 's':
	        fprintf(stdout, "-s");
	        break;
	    case 'm':
	        fprintf(stdout, "-m");
	        break;
	    default:
	        break;
	}
	//prints to stdout a comma-separated-value (CSV) record

	free(id);
	free(pool);

//	char name[20] = "";
//	if (strlen(yieldopt) == 0)
//	{
//		strcat(name, "none");
//	}
//	else
//	{
//		strcat(name, yieldopt);
//	}
//
//	switch (opt_sync)
//	{
//		case 'm':
//			strcat(name, "-m");
//			break;
//		case 's':
//			strcat(name, "-s");
//			break;
//		default:
//			strcat(name, "-none");
//	}

	int operations = threads * iterations * 3;
	long long diff = 1000000000 * (end.tv_sec - begin.tv_sec) + end.tv_nsec - begin.tv_nsec;
	int average_time_per_operation = diff / operations;
	int average_time_per_lock = lock_time / operations;
//	int num_lists = 1;
	printf(",%d,%d,%d,%d,%lld,%d,%d\n",/*name,*/threads, iterations, num_lists, operations, diff, average_time_per_operation,average_time_per_lock);

//	if(opt_sync == 'm')
//	{
//		for (;i < num_lists; i++)
//		{
//			if( pthread_mutex_destroy(&mutex[i]) != 0 )
//			{
//				fprintf(stderr, "Cannnot destroy mutex[i].\n");
//				exit(1);
//			}
//		}
//	}
	exit(0);
}




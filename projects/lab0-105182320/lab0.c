// NAME: Ziying Yu
// EMAIL: annyu@g.ucla.edu
// ID: 105182320

#include <unistd.h> //read, write, dup, close
#include <signal.h> //signal
#include <fcntl.h> //open, creat
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h> //strerror
#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <getopt.h> //getopt_long(--arg)
#include <errno.h> //errno

#define BUFFERSIZE 1024
char buffer[BUFFERSIZE];

//discuss on the discussion
void sigsegv_handler(int sig)
{
    fprintf(stderr, "%d: %s Segmentation fault is caught.\n", sig, strerror(errno));
    exit(4);
}

struct option args[] = {
    {"input", 1, NULL, 'i'},
    {"output", 1, NULL, 'o'},
    {"segfault", 0, NULL, 's'},
    {"catch", 0, NULL, 'c'},
    {0, 0, 0, 0},
};

int main(int argc, char * argv[])
{
    int fd0, fd1, flag1;
    ssize_t ret, read_in, write_out;
    char *ptr;
    
    while( (ret = getopt_long(argc, argv, "", args, NULL)) != -1)
    {
        switch (ret)
        {
            case 'i':
                fd0 = open(optarg, O_RDONLY); //read-only
                if(fd0 < 0)
                {
                    fprintf(stderr, "%s: --input=filename option caused the problem.\n", strerror(errno));
                    fprintf(stderr, "%s: %s file could not be opened.\n", optarg, strerror(errno));
                    exit(2);
                }
                else
                {
                    close(0);
                    dup(fd0);
                    close(fd0);
                }
                break;
            case 'o':
                fd1 = creat(optarg, 0666); //file owner has read, write and execute permission
                if (fd1 < 0)
                {
                    fprintf(stderr, "%s: --output=filename option caused the problem.\n", strerror(errno));
                    fprintf(stderr, "%s: %s file could not be created.\n", optarg, strerror(errno));
                    exit(3);
                }
                else
                {
                    close(1);
                    dup(fd1);
                    close(fd1);
                }
                break;
            case 's':
                flag1 = 1;
//                char *ptr = NULL;
//                *ptr = 'A';
                break;
            case 'c':
                ptr = NULL;
                signal(SIGSEGV, sigsegv_handler);
                (*ptr)=0;
                break;
            default:
                fprintf(stderr, "%s: Unrecognized argument, correct usage line: --input=filename; --output=filename; --segfault; --catch.\n", strerror(errno));
                exit(1);
        }
    }
        
        if(flag1 == 1)
        {
            char *ptr = NULL;
            *ptr = 'A';
        }
        
        
        while(1)
        {
            read_in = read(0, buffer, BUFFERSIZE);
            if(read_in == 0)
            {
                break;
            }
            if(read_in == -1)
            {
                perror("Could not read from stdin.\n");
                break;
            }
            
            write_out = write(1, buffer, read_in);
            if(write_out < 0 )
            {
                fprintf(stderr, "%s: Output file could not be wrote.\n", strerror(errno));
                exit(3);
            }
        }
        
        exit(0);
        
}

//NAME: Ziying Yu
//EMAIL: annyu@g.ucla.edu
//ID: 105182320

#include <termios.h> //termios(3), tcgetattr(3), tcsetattr(3)
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> //waitpid(2)
#include <signal.h> //kill(3)
#include <string.h>
#include <poll.h> //poll(2)
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h> //getopt_long(--arg)

#define BUFFERSIZE 256
char buffer[BUFFERSIZE];

struct termios ori;

int shell_flag = 0;

struct pollfd pollfds[2];

void pterminal_restore(void)
{
    tcsetattr (0, TCSANOW, &ori);
//    if (shell_flag)
//    {
        int status = 0;
    /*0: wait for any child process whose process group ID is equal to that of the calling process (cite: http://www.tutorialspoint.com/unix_system_calls/waitpid.htm)*/
        waitpid(0, &status, 0);
        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", /*(status & 0x007f), (status & 0xff00)*/WTERMSIG(status), WEXITSTATUS(status));
//    }
}

void terminal_restore(void)
{
    tcsetattr(0,TCSANOW, &ori);
    if ( (tcsetattr(0,TCSANOW, &ori)) <0 )
    {
        fprintf(stderr, "Fail to set the state of the ori teminal specified by STDIN_FILENO.\n");
        exit(1);
    }
}

void terminal_setup(void)
{
    struct termios tmp;
    
    if( !isatty(0) )
    {
        fprintf(stderr, "Exit terminal modes in unusable state.\n");
        exit(1);
    }
    
    tcgetattr(0, &ori);
    if( (tcgetattr(0, &ori)) < 0)
    {
        fprintf(stderr, "Fail to get the state of the ori teminal specified by STDIN_FILENO.\n");
        exit(1);
    }
    
    atexit(terminal_restore);
    
    tcgetattr(0, &tmp);
    if( (tcgetattr(0, &tmp)) < 0)
    {
        fprintf(stderr, "Fail to get the state of the tmp teminal specified by STDIN_FILENO.\n");
        exit(1);
    }
    
    tmp.c_iflag=ISTRIP, tmp.c_oflag=0, tmp.c_lflag=0;
    tcsetattr(0, TCSANOW,&tmp);
    if ( (tcsetattr(0,TCSANOW, &tmp)) <0 )
    {
        fprintf(stderr, "Fail to set the state of the tmp teminal specified by STDIN_FILENO.\n");
        exit(1);
    }
}

void read_write(int input, int output)
{
    ssize_t ret;
    unsigned int i;
    char c;

    while(1)
    {
        ret = read(input, buffer, BUFFERSIZE);
        if(ret<0)
        {
            fprintf(stderr, "Error on read from input in non-shell mode.\n");
            exit(1);
        }
        
        for(i = 0; i < ret; i++)
        {
            c = buffer[i];
            if( c =='\r' || c =='\n')
            {
                write(output, "\r\n", 2);
            }
            
            else if(c == 0x04)
            {
                write(output, "^D", 2);
                //restore;
                terminal_restore();
                exit(0);
            }
            
            else
            {
                write(output,buffer+i,1);
            }
        }
    }
}

struct option args[] = {
    {"shell", 1, NULL, 's'},
    {0, 0, 0, 0},
};


int main(int argc, char * argv[])
{
    terminal_setup();
    
    
    ssize_t option;
    int shell_flag;
    char* argument_shell;
    
    while(( option = getopt_long(argc, argv, "", args, NULL)) != -1)
    {
        switch(option)
        {
            case 's':
                argument_shell = optarg;
                shell_flag = 1;
                break;
            default:
                fprintf(stderr, "Unrecognized argument.\n");
                exit(1);
        }
    }
    
    if (shell_flag != 1)
    {
        read_write(0,1);
        terminal_restore();
        exit(0);
    }
    //TODO: may put in front
    if (shell_flag == 1)
    {
        int to_shell[2];
        int from_shell[2];
//        pipe(to_shell);
        
        if( pipe(to_shell) <0 )
        {
            fprintf(stderr, "Failed to pipe to the shell");
            exit(1);
        }
        
        if( pipe(from_shell) <0 )
        {
            fprintf(stderr, "Failed to pipe from the shell");
            exit(1);
        }
        
        pid_t pid = fork();
    
        if (pid < 0)
        {
            fprintf(stderr, "Fail to create a fork.\n");
            exit(1);
        }
        //child process
        if( pid ==0 )
        {
            //0:readin 1:writeout
            close(to_shell[1]);//close unused end of pipes
            close(from_shell[0]); ///
            
            close(0);
            dup(to_shell[0]);
            close(to_shell[0]);

            close(1);
            dup(from_shell[1]);
            close(from_shell[1]);

            close(2);
            dup(from_shell[1]);
            close(from_shell[1]);
            
            execlp(argument_shell,argument_shell,NULL);

            if( (execlp(argument_shell,argument_shell,NULL) ) <0 )
            {
                
                fprintf(stderr,"The call to execlp() was not successful.\n");
                exit(1);
            }
        }
    
        //parent process
        else
        {
            close(to_shell[0]); //close unused end of pipes
            close(from_shell[1]);
            
    
            pollfds[0].fd = 0;
            pollfds[0].events = POLLIN | POLLHUP | POLLERR;

            pollfds[1].fd = from_shell[0];
            pollfds[1].events = POLLIN | POLLHUP | POLLERR;
            
            ssize_t count;
            int i;
            
            while(1)
            {
                poll(pollfds, 2, -1);
                if( (poll(pollfds, 2, -1)) <0 )
                {
                    fprintf(stderr, "error in poll().\n");
                    exit(1);
                }
                if (pollfds[0].revents & POLLIN)
                {
                    count=read(0, buffer, BUFFERSIZE);
                    if(count<0)
                    {
                        fprintf(stderr, "Error on read from parent input in shell mode.\n");
                        exit(1);
                    }
                    
                    for(i = 0; i < count; i++)
                    {
                        char c = buffer[i];
                        if( c == '\r' || c == '\n')
                        {
                            write(to_shell[1], "\n", 1);
                            write(1, "\r\n", 2);
                        }
                        
                        else if(c == 0x03)
                        {
                            write(1, "^C", 2);
                            kill(pid, SIGINT);
                        }
                        
                        //EOF from terminal
                        else if(c == 0x04)
                        {
                            write(1, "^D", 2);
                            close(to_shell[1]); //read from pipe;
//                            close(from_shell[0]);
//                            pterminal_restore();
//                            exit(0);
                        }
                        
                        else
                        {
                            write(1, buffer+i, 1);
                            write(to_shell[1],buffer+i, 1);
                        }
                    }
                }
                
                if(pollfds[1].revents & POLLIN)
                {
                    count=read(from_shell[0], buffer, sizeof(buffer));
                    if(count<0)
                    {
                        fprintf(stderr, "Error on read from child input in shell mode.\n");
                        exit(1);
                    }
                    
                    for(i = 0; i < count; i++)
                    {
                        char c = buffer[i];
                        if( c == '\n')
                        {
                            write(1, "\r\n", 2);
                        }
                        
//                        EOF from shell
                        else if( c == 0x04)
                        {
                            close(from_shell[0]);
//                            pterminal_restore();
//                            exit(0);
                        }
                        else
                        {
                            write(1,buffer+i,1);
                        }
                    }
                }
                
                if(pollfds[0].revents & (POLLHUP | POLLERR))
                {
//                    close(to_shell[1]);
//                    //close(from_shell[0]);
//                    pterminal_restore();
                    exit(1);
                }
                
                if(pollfds[1].revents & (POLLHUP | POLLERR))
                {
//                    close(to_shell[1]);
//                    close(from_shell[0]);
                    pterminal_restore();
                    exit(0);
                }
            }
        }
        
    }
//    terminal_restore();
    exit(0);
}

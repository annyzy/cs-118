//NAME: ZIYING YU
//EMAIL: annyu@g.ucla.edu
//ID: 105182320

#include <sys/stat.h>
#include <fcntl.h>
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

#include <string.h>
#include <arpa/inet.h> //htons();
#include <stdlib.h> // atoi();
#include <mcrypt.h>
#include <sys/socket.h>

#define BUFFERSIZE 256
char buffer[BUFFERSIZE];

//struct termios ori;

int port_flag ;
int log_flag ;
int encrypt_flag;

MCRYPT encrypt_fd;
MCRYPT decrypt_fd;


struct pollfd pollfds[2];

char *iv;
char *mykey;
char* key_filename;
off_t size_of_key_buf;

int newsocket_fd, listenfd;
int portarg;

pid_t pid;
int to_shell[2];
int from_shell[2];

struct sockaddr_in serv_addr, cli_addr;
unsigned int cli_len;
int bind_fd;

void close_session(MCRYPT session)
{
//    mcrypt_generic_deinit(session);
    if ( (mcrypt_generic_deinit(session)) < 0)
    {
        fprintf(stderr, "Error on terminates.\n");
        exit(1);
    }
	
    mcrypt_module_close(session);
}

void harvest(void)
{
	int status = 0;
	/*0: wait for any child process whose process group ID is equal to that of the calling process (cite: http://www.tutorialspoint.com/unix_system_calls/waitpid.htm)*/
	waitpid(pid, &status, 0);
	fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", /*(status & 0x007f), (status & 0xff00)*/WTERMSIG(status), WEXITSTATUS(status));

	if(encrypt_flag==1)
	{
		free(iv);
    	free(mykey);
        close_session(encrypt_fd);
        close_session(decrypt_fd);
	}
	close(newsocket_fd);
}

int server_connect(unsigned int port_num)
{
//    struct sockaddr_in serv_addr, cli_addr;
//    unsigned int
    cli_len = sizeof(struct sockaddr_in);
//	int bind_fd;
	
    listenfd = socket(AF_INET, SOCK_STREAM,0);
    if(listenfd < 0)
    {
        fprintf(stderr, "Error on");
        exit(1);
    }
	
    //fills the first struct sockaddr_in bytes of the memory area &serv_addr with the constant byte 0
    memset(&serv_addr, 0, sizeof(struct sockaddr_in) );
    serv_addr.sin_family = AF_INET;
	
    serv_addr.sin_addr.s_addr = INADDR_ANY;
	
    serv_addr.sin_port = htons(port_num);
    bind_fd = bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if ( bind_fd  < 0)
	{
        perror("Error on bind().\n");
        exit(1);
	}
	listen(listenfd,5);
	newsocket_fd = accept(listenfd, (struct sockaddr *)&cli_addr, &cli_len);
	if ( newsocket_fd < 0)
	{
		fprintf(stderr, "Error on accept().\n");
        exit(1);
	}
    return newsocket_fd;
}



void sig_handler(int sig)
{
    if (sig == SIGPIPE)
    {
        exit(0);
    }
}




struct option args[] = {
    {"port", 1, NULL, 'p'},
    {"encrypt", 1, NULL, 'e'},
//    {"host", 1, NULL, 'h'},
    {0, 0, 0, 0},
};
		
int main(int argc, char * argv[])
{
    ssize_t option;
	
	atexit(harvest);

	int portarg;
//	char *key_opt;
    while(( option = getopt_long(argc, argv, "", args, NULL)) != -1)
    {
        switch(option)
        {
            case 'p':
            	port_flag = 1;
                portarg = atoi(optarg);
                break;
            case 'e':
                encrypt_flag = 1;
				key_filename = optarg;
                break;
            default:
                fprintf(stderr, "Unrecognized argument.\n");
                exit(1);
        }
    }
	
	newsocket_fd = server_connect(portarg);
	
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
	
	pid = fork();
	
	if (pid < 0)
	{
		fprintf(stderr, "Fail to create a fork.\n");
		exit(1);
	}
	
	if( pid ==0 )
	{
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
		
		execl("/bin/bash","/bin/bash", NULL);
		
		fprintf(stderr,"The call to execl() was not successful.\n");
		exit(1);
	}
	
	if( pid > 0)
	{
		close(to_shell[0]); //close unused end of pipes
		close(from_shell[1]);
		
		int key_fd,ret;
		//	off_t size_of_key_buf;
		struct stat key_file;
		
		if(encrypt_flag)
		{
			key_fd = open(key_filename, O_RDONLY);
			
			if(fstat(key_fd, &key_file) < 0 )
			{
				fprintf(stderr, "Error on open my.key file.\n");
				exit(1);
			}
			
			mykey = (char*)malloc(key_file.st_size * sizeof(char));
			ret = read(key_fd, mykey, key_file.st_size);
			if (ret < 0)
			{
				fprintf(stderr, "Error on open my.key file.\n");
				exit(1);
			}
			
			size_of_key_buf = key_file.st_size;
			
			//				MCRYPT encrypt_fd, decrypt_fd;

			
			if(encrypt_flag==1)
			{
			
				encrypt_fd = mcrypt_module_open("twofish", NULL, "cfb",NULL);
				if( encrypt_fd == MCRYPT_FAILED)
				{
					fprintf(stderr, "Fail to decrypt\n");
					exit(1);
				}
				decrypt_fd = mcrypt_module_open("twofish", NULL, "cfb",NULL);
				if( decrypt_fd == MCRYPT_FAILED)
				{
					fprintf(stderr, "Fail to decrypt\n");
					exit(1);
				}
				iv = malloc(mcrypt_enc_get_iv_size(encrypt_fd));
				memset(iv,0,mcrypt_enc_get_iv_size(encrypt_fd));

				mcrypt_generic_init(encrypt_fd,mykey,size_of_key_buf,iv);
				if ( (mcrypt_generic_init(encrypt_fd,mykey,size_of_key_buf,iv)) < 0)
				{
					fprintf(stderr, "Error on initializing session.\n");
					exit(1);
				}
				mcrypt_generic_init(decrypt_fd,mykey,size_of_key_buf,iv);
				if ( (mcrypt_generic_init(decrypt_fd,mykey,size_of_key_buf,iv)) < 0)
				{
					fprintf(stderr, "Error on initializing session.\n");
					exit(1);
				}

			}
			
		}
		
		pollfds[0].fd = newsocket_fd;
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
				count=read(newsocket_fd, buffer, BUFFERSIZE);
				if(count<0)
				{
					fprintf(stderr, "Error on read from parent input in shell mode.\n");
					exit(1);
				}
//				if(encrypt_flag)
//				{
//					decrypt_buffer(decrypt_fd, buffer,1);
//				}
				for(i = 0; i < count; i++)
				{
					
//					write(to_shell[1],buffer+i, 1);
					
					if(encrypt_flag)
					{
						
						if ( (mdecrypt_generic(decrypt_fd, buffer+i, 1)) != 0)
						{
							fprintf(stderr, "Fail on decryption.\n");
							exit(1);
						}
					}
					char c = buffer[i];
					if( c == '\r' || c == '\n')
					{
						write(to_shell[1], "\n", 1);
						
					}
					
					else if(c == 0x03)
					{
						write(newsocket_fd, buffer+1, 1);
						kill(pid, SIGINT);
					}
					
					//EOF from terminal
					else if(c == 0x04)
					{
						write(newsocket_fd, buffer+1, 1);
						close(to_shell[1]); //read from pipe;
					}
					
					else
					{
						
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
				
						char cr_new[2] = {'\r','\n'};
						if (encrypt_flag)
						{
							
							if ( (mcrypt_generic(encrypt_fd, cr_new, 2)) != 0)
							{
								fprintf(stderr, "Error on encryption.\n");
								exit(1);
							}
							
						}
						write(newsocket_fd, cr_new, 2);
					}
					
					//                        EOF from shell
					else if( c == 0x04)
					{
						exit(0);
					}
					else
					{
						if(encrypt_flag)
						{
							
							
							if ( (mcrypt_generic(encrypt_fd, buffer+i, 1)) != 0)
							{
								fprintf(stderr, "Error on encryption.\n");
								exit(1);
							}
						}
	
						write(newsocket_fd,buffer+i,1);
					}
				}
				
				memset(buffer, 0, sizeof(buffer));
			}
			
//			if(pollfds[0].revents & (POLLHUP | POLLERR))
//			{
//				exit(0);
//			}
			
			if(pollfds[1].revents & (POLLHUP | POLLERR))
			{
				//			harvest();
				exit(0);
			}
		}
	}
	
	
	
    exit(0);
}

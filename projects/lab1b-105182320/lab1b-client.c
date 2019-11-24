//NAME: ZIYING YU
//EMAIL: annyu@g.ucla.edu
//ID: 105182320

#include <sys/stat.h>
#include <fcntl.h>
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

#include <string.h>
#include <arpa/inet.h> //htons();
#include <stdlib.h> // atoi();
#include <mcrypt.h>
#include <netdb.h>
#include <sys/socket.h>

#define BUFFERSIZE 256
char buffer[BUFFERSIZE];

struct termios ori;

int port_flag = 0;
int log_flag = 0;
int encrypt_flag = 0;

MCRYPT encrypt_fd; //
MCRYPT decrypt_fd; //

char *iv;
char *mykey;
off_t size_of_key_buf;

struct stat key_file;

int port_num, socket_fd, log_fd;
struct pollfd pollfds[2];

struct sockaddr_in serv_addr;

int key_fd;

int client_connect(char * host_name, unsigned int port)
{
    struct sockaddr_in serv_addr;
	int connect_fd,sockfd;
	
    sockfd = socket(AF_INET, SOCK_STREAM,0);
	
    if(sockfd == -1)
    {
        fprintf(stderr, "Error on socket");
        exit(1);
    }
	
    struct hostent *server = gethostbyname(host_name);
    if (server == NULL)
    {
        fprintf(stderr, "Error on server");
        exit(1);
    }
	
    //fills the first struct sockaddr_in bytes of the memory area &serv_addr with the constant byte 0
    memset(&serv_addr, 0, sizeof(struct sockaddr_in) );
    serv_addr.sin_family = AF_INET;
    //copies server->length bytes from memory area server->h_addr to memory area &serv_addr.sin_addr.s_addr
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
	
    connect_fd = connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if ( connect_fd < 0)
    {
            perror("Error on connect");
            exit(1);
    }
	
    return sockfd;
}

void close_session(MCRYPT session)
{
//    mcrypt_generic_deinit(session);
    if ( (mcrypt_generic_deinit(session)) < 0)
    {
        fprintf(stderr, "%s:Error on terminates.\n", strerror(errno));
        exit(1);
    }
	
    mcrypt_module_close(session);
}

void terminal_restore()
{
    tcsetattr(0,TCSANOW, &ori);
    if ( (tcsetattr(0,TCSANOW, &ori)) <0 )
    {
        fprintf(stderr, "Fail to set the state of the ori teminal specified by STDIN_FILENO.\n");
        exit(1);
    }
	
    if(log_flag==1)
    {
        close(log_fd);
    }
	
    if(encrypt_flag==1)
    {
    	free(iv);
    	free(mykey);
        close_session(encrypt_fd);
        close_session(decrypt_fd);
    }
	
    close(socket_fd);
}

void terminal_setup(void)
{
    struct termios tmp;
	
    if( !isatty(0) )
    {
        fprintf(stderr, "Exit terminal modes in unusable state.\n");
        exit(1);
    }
        
//    tcgetattr(0, &ori);
    if( (tcgetattr(0, &ori)) < 0)
    {
        fprintf(stderr, "Fail to get the state of the ori teminal specified by STDIN_FILENO.\n");
        exit(1);
    }
        
    atexit(terminal_restore);
        
//    tcgetattr(0, &tmp);
    if( (tcgetattr(0, &tmp)) < 0)
    {
        fprintf(stderr, "Fail to get the state of the tmp teminal specified by STDIN_FILENO.\n");
        exit(1);
    }
//	struct termios tmp = ori;
    tmp.c_iflag=ISTRIP, tmp.c_oflag=0, tmp.c_lflag=0;
//    tcsetattr(0, TCSANOW,&tmp);
    if ( (tcsetattr(0,TCSANOW, &tmp)) <0 )
    {
        fprintf(stderr, "Fail to set the state of the tmp teminal specified by STDIN_FILENO.\n");
        exit(1);
    }
}
        
        
//MCRYPT encrypt_fd init_session(char *keybuf, int keylen)
//{
//    //    char key_buf[];
//    encrypt_session = mcrypt_module_open("twofish", NULL, "cfb",NULL);
//    if( encrypt_session == MCRYPT_FAILED)
//    {
//        fprintf(stderr, "Fail to ....\n");
//        exit(1);
//    }
//
//    char *iv;
//    iv = malloc(mcrypt_enc_get_iv_size(encrypt_session));
//    memset(iv,0,mcrypt_enc_get_iv_size(encrypt_session));
//    mcrypt_generic_init(encrypt_session,keybuf,keylen,iv);
//    if ( (mcrypt_generic_init(encrypt_session,keybuf,keylen,iv)) < 0)
//    {
//        fprintf(stderr, "Error on initializing session.\n");
//        exit(1);
//    }
//
//    return encrypt_fd;
//}


MCRYPT init_session()
{
    //    char key_buf[];
    MCRYPT session;
    session = mcrypt_module_open("twofish", NULL, "cfb",NULL);
    if( session == MCRYPT_FAILED)
    {
        fprintf(stderr, "Fail to ....\n");
        exit(1);
    }

    iv = malloc(mcrypt_enc_get_iv_size(session));
    memset(iv,0,mcrypt_enc_get_iv_size(session));

    mcrypt_generic_init(session,mykey,size_of_key_buf,iv);
    if ( (mcrypt_generic_init(session,mykey,size_of_key_buf,iv)) < 0)
    {
        fprintf(stderr, "Error on initializing session.\n");
        exit(1);
    }

    return session;
}

void encrypt_buffer(MCRYPT session, char *ori_buf, unsigned long len)
{
//	mcrypt_generic(session, ori_buf, len);
	
	if ( (mcrypt_generic(session, ori_buf, len)) != 0)
	{
		fprintf(stderr, "Error on encryption.\n");
		exit(1);
	}
}
		
void decrypt_buffer(MCRYPT session, char *ori_buf, unsigned long len)
{
//    mcrypt_generic(session, ori_buf, len);
	
    if ( (mdecrypt_generic(session, ori_buf, len)) != 0)
    {
        fprintf(stderr, "Fail on decryption.\n");
        exit(1);
    }
}

struct option args[] = {
    {"port", 1, NULL, 'p'},
    {"log", 1, NULL, 'l'},
    {"encrypt", 1, NULL, 'e'},
//    {"host", 1, NULL, 'h'},
    {0, 0, 0, 0},
};

int main(int argc, char * argv[])
{
//    terminal_setup();
    ssize_t option;
//    char *key_filename;
    while(( option = getopt_long(argc, argv, "", args, NULL)) != -1)
    {
        switch(option)
        {
            case 'p':
            	port_flag = 1;
                port_num = atoi(optarg);
                break;
            case 'l':
            	log_flag = 1;
                log_fd = creat(optarg, 0666);
                if (log_fd <0 )
                {
					fprintf(stderr, "%s:%s Error on creat logfile.\n",optarg,strerror(errno));
                    fprintf(stderr, "%s:Error on creat.\n",strerror(errno) );
                    exit(1);
                }
                break;
            case 'e':
                encrypt_flag = 1;
				key_fd = open(optarg,0666);
				if (key_fd < 0)
				{
					fprintf(stderr, "Error on creat keyfile.\n");
					fprintf(stderr, "Error on open.\n");
                    exit(1);
				}
				if(fstat(key_fd, &key_file) < 0 )
				{
					fprintf(stderr, "Error on open my.key file.\n");
					exit(1);
				}
				
				int ret;
				mykey = malloc(key_file.st_size * sizeof(char));
				if (mykey == NULL)
				{
					fprintf(stderr, "Error on allocating memory.\n");
					exit(1);
				}
				ret = read(key_fd, mykey, key_file.st_size * sizeof(char));
				if (ret < 0)
				{
					fprintf(stderr, "Error on open my.key file.\n");
					exit(1);
				}
				close(key_fd);
				
				//
				size_of_key_buf = key_file.st_size;

                break;
            default:
                fprintf(stderr, "Unrecognized argument.\n");
                exit(1);
        }
    }
    if (port_flag < 0)
    {
      fprintf(stderr, "Error on port option.\n");
      exit(1);
	}
	
	terminal_setup();
    socket_fd = client_connect("localhost", port_num);
	
	pollfds[0].fd = 0;
	pollfds[0].events = POLLIN | POLLHUP | POLLERR;
        
	pollfds[1].fd = socket_fd;
	pollfds[1].events = POLLIN | POLLHUP | POLLERR;
	
	if(encrypt_flag==1)
	{
		
			encrypt_fd = mcrypt_module_open("twofish", NULL, "cfb",NULL);
			if( encrypt_fd == MCRYPT_FAILED)
			{
				fprintf(stderr, "Fail to ....\n");
				exit(1);
			}
			decrypt_fd = mcrypt_module_open("twofish", NULL, "cfb",NULL);
			if( decrypt_fd == MCRYPT_FAILED)
			{
				fprintf(stderr, "Fail to ....\n");
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
	
	ssize_t count;
	int i;
	
//	int key_fd,ret;
//	struct stat key_file;
	
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

					write(1, "\r\n", 2);

					if(encrypt_flag==1)
					{

						if ( (mcrypt_generic(encrypt_fd, buffer+i, 1)) != 0)
						{
							fprintf(stderr, "Error on encryption.\n");
							exit(1);
						}
						
					}
					write(socket_fd,buffer+i, 1);
					if(log_flag==1)
					{

//						write(log_fd, "SENT 1 bytes: ", sizeof("SENT 1 byte: "));
						write(log_fd,  buffer+i, 1);
						write(log_fd, "\n",1);
					}
				}

				else
				{
					write(1, buffer+i, 1);
					if (encrypt_flag == 1)
					{
						if ( (mcrypt_generic(encrypt_fd, buffer+i, 1)) != 0)
						{
							fprintf(stderr, "Error on encryption.\n");
							exit(1);
						}
						
					}
					write(socket_fd,buffer+i, 1);
					if(log_flag==1)
					{

						write(log_fd, "SENT 1 bytes: ", sizeof("SENT 1 byte: "));
						write(log_fd,  buffer+i, 1);
						write(log_fd, "\n",1);
					}
				}
			}
		}
            
		if(pollfds[1].revents & POLLIN)
		{
			count=read(socket_fd, buffer, sizeof(buffer));
			
			if(count==0)
			{
				exit(0);
			}
			if(count<0)
			{
				fprintf(stderr, "Error on read from child input in shell mode.\n");
				exit(1);
			}
			
			for(i = 0; i < count; i++)
			{
//				char c = buffer[i];
				
				if(log_flag == 1)
				{
					write(log_fd, "RECEIVED 1 bytes: ", sizeof("RECEIVED 1 bytes: "));
					write(log_fd,  buffer+i, 1);
					write(log_fd, "\n",1);
				}
				
				if (encrypt_flag ==1)
				{
				
					if ( (mdecrypt_generic(decrypt_fd, buffer+i, 1)) != 0)
					{
						fprintf(stderr, "Fail on decryption.\n");
						exit(1);
					}
					write(1, buffer+i, 1);
				}
				else
				{
					write(1, buffer+i, 1);
				}
			}
			
			memset(buffer, 0, sizeof(buffer));
		}
		
		if(pollfds[0].revents & (POLLHUP | POLLERR))
		{
			exit(0);
		}
		
		if(pollfds[1].revents & (POLLHUP | POLLERR))
		{
			close(socket_fd);
			exit(0);
		}
	}

    terminal_restore();
    exit(0);
}

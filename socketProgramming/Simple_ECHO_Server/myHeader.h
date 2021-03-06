#include<stdio.h>
#include<stdlib.h> // needed for malloc
#include<string.h> //needed for string functions.

#include<sys/socket.h> // needed for socket( PF_INET, SOCK_STREAM, 0 );
#include<arpa/inet.h> //  needed for  bind
#include<unistd.h> //needed for close()

#include <errno.h>

/*Header File for threads */
#include<pthread.h> 
#include<signal.h>


void *workerThread(void* _socket);

#define RET_ERROR (-1)
#define PORT 3000

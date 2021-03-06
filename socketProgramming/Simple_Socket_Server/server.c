#include<stdio.h>
#include<stdlib.h> // needed for malloc
#include<string.h> //needed for string functions.
#include<sys/socket.h> // needed for socket( PF_INET, SOCK_STREAM, 0 );
#include<arpa/inet.h> //  needed for  bind

#define RET_ERROR (-1)

int main(){

    int listener_d = socket( PF_INET, SOCK_STREAM, 0 );
    if(  listener_d == RET_ERROR ){
	perror("ERROR:Can't open socket.\n'");
    }
    
    struct sockaddr_in name;
    name.sin_family = PF_INET;
    name.sin_port = (in_port_t)htons(3000);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int ret = bind(listener_d, (struct sockaddr *) &name, sizeof(name) );
    if( ret == RET_ERROR ){
	perror("ERROR:Can't bind to the socket.\n'");
	
	}
	
    if( listen(listener_d, 10) == RET_ERROR  ){
	perror("ERROR:Can't listen.\n'");
	}
	
    printf("Waiting for the Connection\n");
	
    struct sockaddr_storage client_addr;
    unsigned int address_size = sizeof( client_addr);
    int connect_d = accept(listener_d, (struct sockaddr *) &client_addr, &address_size);
    if( connect_d == RET_ERROR ){
	perror("ERROR:Can't open secondary socket.'\n");
	}
	
    char* msg = NULL;
    msg = (char*) malloc(40*sizeof(char));
    strcpy(msg,"Welcome to the simple Socket server. ;)");
    
    if ( send(connect_d, msg, strlen(msg), 0 ) == RET_ERROR ){
	perror("ERROR: send failed.\n");
	}
	
    free(msg);
    
    
    return 0;
    }

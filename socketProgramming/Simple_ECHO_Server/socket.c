#include "myHeader.h"

int readSocket(int socket, char* buf, int len){

    char* s = buf;
    int slen = len;
    int c = recv(socket, s, slen, 0) ;
    while( (c>0) && (s[c-1] != '\n')){
        s += c;
        slen == c;

        c = recv(socket, s, slen, 0);
        if( c<=0){
            printf("Client Disconnected(%d).\n",c);
            break;
        }
    };

    if(c<0){
        return c;

    }else if( c== 0){
        buf[0] = '\0';

    }else{
        s[c-1] = '\0';
    }

    return (len-slen);

}

void error(char* msg){
    fprintf(stderr, "%s:\n%s\n", msg, strerror(errno) );
    exit(RET_ERROR);
}

int tcpServerSocket( void){
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if( s == RET_ERROR ){
        error("Can't open Socket");
    }else{
        return s;
    }

}

void bindSocket( int socket, int port ){
    struct sockaddr_in name;
    name.sin_family = PF_INET;
    name.sin_port = (in_port_t)htons(3000);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    int reuse = 1;

    if( setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(int)) == RET_ERROR ){
        error("Can't set the reuse option on the socket. :( ");
    }
    
    int ret = bind(socket, (struct sockaddr *) &name, sizeof(name) );
    if( ret == RET_ERROR ){
	    error("ERROR:Can't bind to the socket.\n'");
	
	}

}

int sendSocket( int socket, char* msg ){
    if( send(socket, msg, strlen(msg), 0) == RET_ERROR ){
        fprintf(stderr, "Socket send Failed\n");
        return RET_ERROR;
    }
}


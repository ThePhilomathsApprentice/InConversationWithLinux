#include "myHeader.h"

int listener_d = 0;

int readSocket(int socket, char *buf, int len);
void error(char *msg);
int tcpServerSocket(void);
void bindSocket(int socket, int port);
int sendSocket(int socket, char *msg);

int catch_signal(int sig, void (*handler)(int))
{
    struct sigaction action;
    action.sa_handler = handler;

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    return (sigaction(sig, &action, NULL));
}

void handleSignal(int sig)
{

    if (listener_d)
    {
        printf("Got %d, Closing the Server Socket:%d\n", sig, listener_d);
        if (close(listener_d) == RET_ERROR)
        {
            error("Failed to close the socket.\n");
        }
    }
    printf("B Bye!\n");
    exit(0);
}

void *workerThread(void* _socket)
{
    int socket = *((int*)_socket); 

    char recvBuff[1024];

    while (1)
    {

        if (readSocket(socket, recvBuff, sizeof(recvBuff)) == RET_ERROR)
        {
            close(socket);

            fprintf(stderr, "ERROR:reading from socket failed, Closed Client\n");
            break;
        }
        else
        {
            printf("%s\n", recvBuff);
            if (sendSocket(socket, recvBuff) == RET_ERROR)
            {
                fprintf(stderr, "ERROR: send failed.\n");
            }
        }

    } //threadCons:while:end

    printf("Closing Socket: %d\n", socket);
    close(socket);

    pthread_exit(0);

} //threadCons:end

int main(int argc, char *argv[])
{

    if (catch_signal(SIGINT, handleSignal) == RET_ERROR)
    {
        error("Can't set the Interrupt Handler\n");
    }

    int opt;
    int port = 0;
    while ((opt = getopt(argc, argv, "p")) != RET_ERROR)
    {
        switch (opt)
        {
        case 'p':
            //port = optarg;
            printf("port selected: %s\n", optarg);
            break;
        case '?':
        default:
            printf("unknown option: %c\n", optopt);
            break;
        }
    }

    listener_d = tcpServerSocket();

    bindSocket(listener_d, PORT);

    if (listen(listener_d, 10) == RET_ERROR)
    {
        error("ERROR:Can't listen.\n'");
    }

    struct sockaddr_storage client_addr;
    unsigned int address_size = sizeof(client_addr);

    printf("Waiting for the Connection\n");

    pthread_t ThId;

    int i = 0;
    int connect_d[10];
    
    do{
        connect_d[i] = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);
        if (connect_d[i] == RET_ERROR)
        {
            error("ERROR:Can't open secondary socket.'\n");
        }
        else
        {
            i++;
            printf("Client Connected:%d ;) \n", connect_d[i]);
            //TODO: create a thread and pass the socket to him to handle the request.
            int ret = pthread_create(&ThId, NULL, workerThread, &connect_d[i]);
            if (ret != 0)
            {
                perror("Error in thread Creation for ThreadConsumer:\n");
                printf("Error Number:%d\n", ret);
                exit(ret);
            }
        }
    }while(i<=10);

    return 0;
}

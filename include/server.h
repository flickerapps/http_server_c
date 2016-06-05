#include <sys/socket.h>
#include <pthread.h>

#ifndef SERVER_H
#define SERVER_H

#define KB 1024
#define SOCKZEROS 8

enum methods_enum{
    HEAD,
    GET,
    POST,
    PUT,
    PATCH,
    DELETE
};
typedef enum methods_enum method_t;


struct cli_request_struct{
    method_t method; /* request method */
    int fd; /* socket file descriptor */
    struct sockaddr_in* saddr;
    socklen_t size; /* sockets size */
};

typedef struct cli_request_struct request_t;

pthread_mutex_t mutex;
int mainSocket; /* Global socket */
pthread_t tid[3]; /* request, (response) send and (response) receive */
int client;
socklen_t socksize;

/* prototypes */

int createServer(int, struct sockaddr_in*);
int runServer(int , struct sockaddr_in, int, int);
int acceptClient(int, struct sockaddr_in);
int serverReady(int, struct sockaddr_in);
int shutdownServer(int);
void* handleAsync(void*);
void* recvAsync(void*);
void* sendAsync(void*);
void interrupt(int);

 /* end */

#endif

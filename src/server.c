
/*local deps*/

#include <server.h>
#include <libstr/libstr.h>
#include <querystr/querystr.h>
/* end */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int createServer(int port, struct sockaddr_in* mysocket){
    int s, opt;
    socksize = sizeof( struct sockaddr );
    mysocket->sin_family = AF_INET;
    mysocket->sin_port = htons(port);
    mysocket->sin_addr.s_addr = htonl(INADDR_ANY);
    memset( mysocket->sin_zero, '\0', SOCKZEROS);

    if( (s = socket(PF_INET, SOCK_STREAM, 0) ) < 0){
        perror("socket");
        exit(1);
    }
    opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    mainSocket = s;
    return s;
}


int runServer(int s, struct sockaddr_in mysocket, int port, int max_cli){
    if( bind(s, (struct sockaddr*)&mysocket, socksize) != 0 ){
        perror("bind");
        exit(1);
    }
    if( listen(s, max_cli) != 0){
        perror("listen");
        exit(1);
    }
    printf("Listening on %s:%d\n",
        inet_ntoa(mysocket.sin_addr), port );
    signal(SIGINT, interrupt);
    signal(SIGKILL, interrupt);
}


int serverReady(int s, struct sockaddr_in mysocket){
    for(;;){
        acceptClient(s, mysocket);
    }
}

int shutdownServer(int s){
    close(s);
    exit(0);
}

int acceptClient(int s, struct sockaddr_in mysocket){
    if( ( client = accept(s, (struct sockaddr*)&mysocket, &socksize) ) < 0){
        perror("accept");
        exit(1);
    }
    request_t client_request;
    client_request.method = GET;
    client_request.fd = client;
    client_request.saddr = &mysocket;
    client_request.size = socksize;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&tid[0], NULL, handleAsync, (void*)&client_request);
}

void interrupt(int sig){
    signal(sig, interrupt);
    printf("\nClosing socket...\nBye!\n");
    close(mainSocket);
    exit(0);
}
void* handleAsync(void* c){
    request_t* client;
    client = (request_t*)c;
    printf("Accepted Connection from %s\n",
        inet_ntoa( (client->saddr)->sin_addr )
    );

    pthread_create(&tid[1], NULL, recvAsync, c);
    pthread_create(&tid[2], NULL, sendAsync, c);

    pthread_join(tid[2], NULL);
    close(client->fd);
    pthread_exit(NULL);
}

void* recvAsync(void *c){
    request_t* client;
    client = (request_t*)c;
    char buffer[KB];
    int recvbytes = recv(client->fd, buffer, KB -1, 0);
    printf("\nRecivied bytes: %d\n", recvbytes);
    int info_len = indexOf('\n', buffer ) - 1;
    char* info_s = substr(buffer, 0, info_len );
    char** req_info = strsplit(' ', info_s);

    var Request = querystr( buffer, '\n', ':', QUERY_HARD);

    setObjectItem(Request, "method", req_info[0]);
    setObjectItem(Request, "url", req_info[1]);
    setObjectItem(Request, "http", req_info[2]);

    pthread_exit(NULL);
}

void* sendAsync(void* c){
    request_t* client;
    client = (request_t*)c;

    char message[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html;charset=utf-8\r\n"
        "Server: micro-httpd\r\n"
        "Set-Cookie: hello=world; lorem=ipsum\r\n"
        "Connection: close\r\n\r\n"
        "Hello World\r\n";
    pthread_join(tid[1], NULL);
    int sendbytes = send( client->fd, message, strlen(message), 0);
    printf("Sent bytes: %d\n", sendbytes);
    pthread_exit(NULL);
}

char* getObjectField(var Object, const char* field){
    if(hasChild(Object, (char*) field)){
        var item = Object->find(Object,(char*)field);
        return item->getString(item);
    }
    else{
        return (char*)"Undefined";
    }
}

int setObjectItem(var Object, const char* key, char* value){
    var item = ObjectCreate((char*)key);
    item->setString(item, value);
    if(isUndefined(Object)){
        Object->setChild(Object, item);
    }
    else{
        Object->appendChild(Object, item);
    }
}

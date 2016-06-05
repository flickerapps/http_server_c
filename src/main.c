#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int port, max_cli;
    if(argc != 3){
        printf("Use:\t%s <port> <max-clients>\n\n", argv[0]);
        exit(1);
    }
    else{
        port = atoi(argv[1]);
        max_cli = atoi(argv[2]);
    }
    int s;
    struct sockaddr_in mysocket;
    s = createServer(port, &mysocket); /* socket() */
    runServer(s, mysocket, port, max_cli); /* bind() and listen() */
    serverReady(s, mysocket); /* accept() */
    shutdownServer(s); /* close() and exit() */

    return 0;
}

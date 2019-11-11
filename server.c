#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "fcntl.h"
#include <unistd.h>
#include "arpa/inet.h"

void ErrorHandling(char* message);

#define N_OF_PORT 5
int main(int argc, char* argv[])
{
    int hServSock, hClntSock;
    struct sockaddr_in servAddr, clntAddr;
    int port;
    socklen_t szClntAddr;
    char message[1024];
    int i,j;
    char str[256];

    if(argc != 4)
    {
	printf("usage : ./client <IP> <PORT> <message>\n");
	return 0;
    }
   
    //connection
    printf("binding port: %d...\n", atoi(argv[2]));

    hClntSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hClntSock == -1)ErrorHandling("socket() error");

    memset(&servAddr, 0x00, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    port = atoi(argv[2]);
    servAddr.sin_port = htons(port);

    if (connect(hClntSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0 )
    {
	ErrorHandling("connect() error");
    }

    //while(read(hClntSock, str, 255) > 0) {puts(str);} 

    if(write(hClntSock, argv[3], strlen(argv[3])) < 0)
    {
	ErrorHandling("write() error");
    }

    printf("send %s successfully\n", argv[3]);

    close(hClntSock);

    return 0;
}

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


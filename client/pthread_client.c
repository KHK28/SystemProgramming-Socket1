#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

typedef struct _TParam
{
    int sock;
    int port;
    char IPv4[50];
}TParam;

void ErrorHandling(char* message);
void *Thread(void* param);

#define N_OF_PORT 5
int main(int argc, char* argv[])
{
    int clientSock[N_OF_PORT];
    struct sockaddr_in servAddr[N_OF_PORT];
    int Ports[N_OF_PORT];
    TParam params[N_OF_PORT];
    pthread_t pthread[N_OF_PORT];
    void * pthread_return;
    int r;
    int i,j;

    if(argc != 2)
    {
        ErrorHandling("Usage: ./pthread_client <SERVER IP>\n");
    }

    printf("Type Server Ports(format:<Port1> <Port2> <Port3> <Port4> <Port5> : \n");
    scanf("%d %d %d %d %d", &Ports[0], &Ports[1], &Ports[2], &Ports[3], &Ports[4]);
    for(i=0;i<N_OF_PORT; i++){
        printf("%d\n",Ports[i]);
    }

    //connection
    for (int i = 0; i < N_OF_PORT; i++) {
	    
        clientSock[i] = socket(PF_INET, SOCK_STREAM, 0);
        if (clientSock[i] == -1)ErrorHandling("socket() error");
	
	    params[i].sock = clientSock[i];
	    params[i].port = Ports[i];
        params[i].IPv4[49] = '\0';
        strncpy(params[i].IPv4, argv[1], 49);
    }

    for (int i = 0; i < 5; i++)
    {
	    if(pthread_create(&pthread[i], NULL, Thread, (void*)&params[i]) != 0)
	    {
		    ErrorHandling("Error creating thread\n");
	    }
    }

    pthread_join(pthread[0], &pthread_return);
    pthread_join(pthread[1], &pthread_return);
    pthread_join(pthread[2], &pthread_return);
    pthread_join(pthread[3], &pthread_return);
    pthread_join(pthread[4], &pthread_return);

    for (int i = 0; i < 5; i++) {
        close(clientSock[i]);
    }
    return 0;
}

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *Thread(void *param)
{

    int clientSock = ((TParam*)param)->sock;
    int port = ((TParam*)param)->port;
    struct sockaddr_in servAddr;
    char log[1024];
    char recvd[256];
    char filename[50];
    FILE *fp;
    time_t t;
    size_t recvlen;
    struct timespec ts;
    struct tm *timestamp;
    int h, m, s;
    long unsigned int ms;

    sprintf(filename, "%d.txt", port);
    fp = fopen(filename, "a+");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(((TParam*)param)->IPv4);
    servAddr.sin_port = htons(port);

    while (1) 
    {
	    while (1) 
	    {
		    if (connect(clientSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)continue;
		    else break;
	    }

	    if((recvlen = recv(clientSock, recvd, 255, 0)) == -1)
            {
		    close(clientSock);
		    continue;
            }
	    timespec_get(&ts, TIME_UTC);
	    timestamp = localtime(&(ts.tv_sec));
	    h = timestamp->tm_hour;
	    m = timestamp->tm_min;
	    s = timestamp->tm_sec;
	    ms = (ts.tv_nsec)/1000000;
	    recvd[recvlen] = '\0';
	    sprintf(log, "%d:%d:%d.%03ld %lu %s\n", h, m, s, ms, strlen(recvd), recvd);
	    puts(log);
	    fputs(log, fp);

	    close(clientSock);
    }
    fclose(fp);
}   

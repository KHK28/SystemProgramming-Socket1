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

typedef struct _TParam
{
    int sock;
    int port;
}TParam;

void ErrorHandling(char* message);
void *ServerThread(void* param);

#define N_OF_PORT 5
int main(int argc, char* argv[])
{
    int hServSock[N_OF_PORT];
    struct sockaddr_in servAddr[N_OF_PORT];
    int servPorts[N_OF_PORT];
    TParam params[N_OF_PORT];
    pthread_t pthread[N_OF_PORT];
    void * pthread_return;

    int r;
    int i,j;
    //init

/*
    for (i = 0; i < N_OF_PORT; i++) {
        for (j = 0; j < 10; j++) presetMSize[i][j] = rand() % 6553;
    }
    int temp = rand() % 5;
    for (i = 0; i < 10; i++)presetMSize[temp][i] *= rand() % 10 + 1;
*/


    printf("Type Server Ports(format:<Port1> <Port2> <Port3> <Port4> <Port5> : \n");
    scanf("%d %d %d %d %d", &servPorts[0], &servPorts[1], &servPorts[2], &servPorts[3], &servPorts[4]);
    for(i=0;i<N_OF_PORT; i++){
        printf("%d\n",servPorts[i]);
    }

    //connection
    for (int i = 0; i < N_OF_PORT; i++) {
        printf("binding port: %d...\n", servPorts[i]);

        hServSock[i] = socket(PF_INET, SOCK_STREAM, 0);
        if (hServSock[i] == -1)		ErrorHandling("socket() error");
	
	params[i].sock = hServSock[i];
	params[i].port = servPorts[i];

        memset(&servAddr[i], 0x00, sizeof(servAddr[i]));

        servAddr[i].sin_family = AF_INET;
        servAddr[i].sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr[i].sin_port = htons(servPorts[i]);

        if (bind(hServSock[i], (struct sockaddr*)&servAddr[i], sizeof(servAddr[i])) < 0 )
	{
		ErrorHandling("bind() error");
	}

        printf("listening port: %d...\n", servPorts[i]);
        if (listen(hServSock[i], 5)<0)	ErrorHandling("listen() error");


         fcntl( hServSock[i], F_SETFL, fcntl( hServSock[i], F_GETFL, 0 ) | O_NONBLOCK );

    }

/*
    printf("accepting...\n");
    while (1) {
        acceptAll = 0;
        for (int i = 0; i < N_OF_PORT; i++) {
            if (acceptState[i] == 1) {
                acceptAll++; continue;
            }

            szClntAddr = sizeof(clntAddr[i]);
            hClntSock[i] = accept(hServSock[i], (struct sockaddr*)&clntAddr[i], &szClntAddr);
            if (hClntSock[i] <0)		continue;//ErrorHandling("accept() error");
            printf("accept port: %d!\n", servPorts[i]);
            acceptState[i] = 1; //connected;
        }
        if (acceptAll == N_OF_PORT) break;
    }

    //send
    while (1) {
        i = rand() % 5;
        //send 횟수
        r = rand() % 10;
        for (int k = 0; k < 1 + (rand() % 50 == 7 ? 10000: 0); k++) {
            //printf("send from port: %d!\n", servPorts[i]);
            for (int j = 0; j < presetMSize[i][r]; j++) message[j] = (rand() % 26) + 'A';
            send(hClntSock[i], message, presetMSize[i][r], 0);

            usleep(rand() % 10);
        }
    }
*/

    for (int i = 0; i < 5; i++)
    {
	if(pthread_create(&pthread[i], NULL, ServerThread, (void*)&params[i]) != 0)
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
        close(hServSock[i]);
    }
    return 0;
}

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *ServerThread(void *param)
{

    int ServerSock = ((TParam*)param)->sock;
    int port = ((TParam*)param)->port;
    int ClientSock;
    struct sockaddr_in ClntAddr;
    socklen_t szClientAddr = sizeof(ClntAddr);
    char log[1024];
    char recvd[256];
    char filename[50];
    FILE *fp;
    time_t t, recvlen;
    struct timespec ts;
    struct tm *timestamp;
    int h, m, s;
    long unsigned int ms;

    sprintf(filename, "%d.txt", port);
    fp = fopen(filename, "a+");

    while (1) 
    {
	while (1) 
	{
		 ClientSock = accept(ServerSock, (struct sockaddr*)&ClntAddr, &szClientAddr);
		 if (ClientSock < 0)continue;
		 else break;
	}
        //receive
	//while (1) 
	//{
		if((recvlen = recv(ClientSock, recvd, 255, 0)) == -1)continue;
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
	//}
	//while(read(ClientSock, recv, 255) > 0){}
	close(ClientSock);
	if(strcmp(recvd, "q") == 0)break;
    }
    fclose(fp);
}   

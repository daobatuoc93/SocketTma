#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
static bool flag = true;
void *roomChat(void *sockfd)
{
    int newsock = *(int *)sockfd;
    char recvbuff[1024];
    int n;
    while (1)
    {
        //receiving data from server
        memset(recvbuff, 0, 1024);
        n = read(newsock, recvbuff, 1024);
        if (n < 0)
        {
            perror("Error reading from socket");
            break;
        }
        else if (n == 0)
        {
            int i = 3;
            flag = false;
            while (i > -1)
            { // 3 second countdown
                printf("\033[A\33[2K\t \nServer has been disconnected!\nTerminate after %d seconds...\n", i);
                i--;
                sleep(1);
            }
            break;
        }

        else
        {
            printf("\n%s\n", recvbuff);
        }
    }
    pthread_exit("\nThread terminated!\n");
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    int *p = &sockfd;
    struct sockaddr_in serv_addr; //struct contain server address that client need to know
    pthread_t thread;
    void *thread_result;
    char sendbuff[256];
    char recvbuff[1024];
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]); 

    //Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("perror opening socket");

    memset(&serv_addr, '0', sizeof(serv_addr));

    //Setting Address server
    serv_addr.sin_family = AF_INET;     //default
    serv_addr.sin_port = htons(portno); //Service Port

    //address ip/domain server
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton perror occured\n");
        return 1;
    }
    printf("\n Please press your username:");
    memset(sendbuff, 0, 255); //init_ buffer
    fgets(sendbuff, 255, stdin);
    char *pos;
    if ((pos = strchr(sendbuff, '\n')) != NULL)
        *pos = '\0';
    //call connect to implement a connection to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n perror : Connect to server failed \n");
        return -1;
    }
    else
    {
        n = write(*p, sendbuff, strlen(sendbuff));
        if (n < 0)
        {
            perror("perror writing to socket");
            return -1;
        }
        else
        {
            printf("\nUsername send!\n");
            int res = pthread_create(&thread, NULL, roomChat, (void *)p);
            if (res != 0)
            {
                perror("Thread create failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    while (1)
    {
        if (flag == false)
        {
            n = pthread_join(thread, &thread_result);
            if (n != 0)
            {
                perror("Thread join failed");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("%s",(char *)thread_result);
            }
            
            printf("Exit!\n");
            break;
        }
        printf("\nPlease enter the message: ");
        memset(sendbuff, 0, 255);
        fgets(sendbuff, 255, stdin);
        n = write(sockfd, sendbuff, strlen(sendbuff));
        if (n < 0)
            perror("perror writing to socket");
    }
    close(sockfd); //close socket
    return 0;
}

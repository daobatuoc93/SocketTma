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
            printf("\nServer has been disconnected!\nClose socket!");
            int i = 5;
            flag = false;
            while (i > -1)
            { // 10 second countdown
                printf("\033[A\33[2K\t Thread will exit after %d...\n", i);
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
    pthread_exit("\nTerminate room!\n");
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    int *p = &sockfd;
    struct sockaddr_in serv_addr; //Cau truc chua dia chi server ma client can biet de ket noi toi
    pthread_t thread;
    void *thread_result;
    char sendbuff[256];
    char recvbuff[1024];

    //Client nhan tham so hostname va port tu dong lenh
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]); //Chuyen cong dich vu thanh so nguyen

    //Tao socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("perror opening socket");

    memset(&serv_addr, '0', sizeof(serv_addr));

    //Thiet lap dia chi cua server de ket noi den
    serv_addr.sin_family = AF_INET;     //Mac dinh
    serv_addr.sin_port = htons(portno); //Cong dich vu

    //Dia chi ip/domain may chu
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton perror occured\n");
        return 1;
    }
    printf("\n Please press your username:");
    memset(sendbuff, 0, 255); //Khoi tao buffer
    fgets(sendbuff, 255, stdin);
    char *pos;
    if ((pos = strchr(sendbuff, '\n')) != NULL)
        *pos = '\0';
    //Goi ham connect de thuc hien mot ket noi den server
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
                perror("Thread join failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    while (1)
    {
        if (flag == false)
        {
            printf("\nExit!");
            break;
        }
        printf("\nPlease enter the message: ");
        memset(sendbuff, 0, 255);
        fgets(sendbuff, 255, stdin);
        n = write(sockfd, sendbuff, strlen(sendbuff));
        if (n < 0)
            perror("perror writing to socket");
    }
    close(sockfd); //Dong socket
    return 0;
}

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <semaphore.h>
/*
    Implement code
*/
typedef struct
{
    int sock;
    char username[30];
    struct sockaddr_in address_client;
    socklen_t socksize;
} connection_t;
struct Node
{
    connection_t client;
    struct Node *next;
};
int count = 0;
struct Node *head = NULL;
struct Node *current = NULL;

sem_t bin_sem;
pthread_cond_t thread_flag_cv;
pthread_mutex_t thread_flag_mutex;

char *get_IP(char *DATA_from_client)
{
    char *a = DATA_from_client + '\0';
    int temp = 0;
    int i, flag = 0;
    char *Ip;
    Ip = malloc((1023) * sizeof(char)); //Data return will < 1kB;if message >1kB,set again plz.
    for (i = 0; i < strlen(a); i++)
    {
        if (a[i] == '\"')
        {
            temp++;
            if (temp == 4)
            {
                memcpy(Ip, &a[flag + 1], i - flag - 1);
                break;
            }
            flag = i;
        }
    }
    return Ip;
}
char *get_DATA(char *DATA_from_client)
{

    char *a = DATA_from_client + '\0';
    int temp = 0;
    int i, flag = 0;
    char *DATA; //Data return will < 1kB;if message >1kB,set again plz.
    DATA = malloc(1024 * sizeof(char));
    for (i = 0; i < strlen(a); i++)
    {
        if (a[i] == '\"')
        {
            temp++;
            if (temp == 8)
            {
                memcpy(DATA, &a[flag + 1], i - flag - 1);
                break;
            }
            flag = i;
        }
    }
    return DATA;
}
//display the list

void printList()
{
    struct Node *ptr;
    ptr = head;
    if (ptr == NULL)
    {
        printf("Linked list is empty.\n");
        return;
    }
    printf("There are %d users in linked list.\n", count);
    printf("\nClient list:\r\n");
    //start from the beginning
    while (ptr->next != NULL)
    {
        printf("\t[Socket:%d with IP username:%s]\n", ptr->client.sock, ptr->client.username); //inet_ntoa(ptr->client->address_client.sin_addr)
        ptr = ptr->next;
    }
    printf("\t[Socket:%d with IP username:%s]\n", ptr->client.sock, ptr->client.username);
}
bool isEmpty()
{
    return head == NULL;
}
int length()
{
    int length = 0;
    struct Node *current;

    for (current = head; current != NULL; current = current->next)
    {
        length++;
    }

    return length;
}

void insertFirst(connection_t *client)
{
    //create a link
    struct Node *link = (struct Node *)malloc(sizeof(struct Node));

    link->client = *client;
    count++;
    //point it to old first node
    if (head == NULL)
    {
        head = link;
        head->next = NULL;
        return;
    }
    link->next = head;

    //point first to new first node
    head = link;
}
void *roomChat(void *newAddr)
{
    char userData[1024]={0};
    struct Node *iClient = newAddr;
    int n;    
    char data[512] = {0};
    while (1)
    {
        printf("\r\nServer's waiting to receive data from client!\n");
        sleep(1);
        n = read(iClient->client.sock, data, 1024);        
        if (n < 0)
        {
            perror("Error reading from client ");
        }
        else if (n == 0)
        {            
            count--;
            printf("\nClient [%s] has been disconnected!\n",iClient->client.username);
            if (count < 1)
            {
                printf("there are no client in this room!\nTerminating room!\n");               
                break;
            }
        }
        else
        {
            const char *clientsay = " said:";
            strcat(userData,iClient->client.username);
            strcat(userData,clientsay);
            strcat(userData,data);
            struct Node *sendClient = head;
            printf("User [%s] said: %s !\n", iClient->client.username, data);
            while (sendClient != NULL)
            {
                if (sendClient != iClient)
                {
                    if (write(sendClient->client.sock, userData, sizeof(data)) > 0)
                    {
                        printf("Senting data to user [%s] succesfully!\n", sendClient->client.username);
                    }
                    else
                    {
                        printf("\nCan't send data to user [%s]", sendClient->client.username);
                    }
                }

                sendClient = sendClient->next;
            }
        }
        memset(data, 0, 512);
        memset(userData, 0, 1024);
    }
    /* read length of message
    This functions is useful when parse the string like Json.
    int recv_size = recv(conn->sock, buffer, 1024, 0);
    char* getData = get_DATA(buffer);
    char* to_IP = get_IP(buffer);
    */
    pthread_exit("\n\r---====Roomchat terminated===----\r\n");
}
void *waitClientAddusername(void *ptr)
{
    connection_t *newAddr = (connection_t *)malloc(sizeof(connection_t));
    newAddr = ptr;
    void *thread_result;
    pthread_t thread;
    char usernamerecv[30];
    memset(usernamerecv, 0, 30);
    int n, i = 5;

    n = read(newAddr->sock, usernamerecv, 30);
    if (n < 0)
    {
        perror("perror reading from client ");
    }
    else if (n == 0)
    {
        printf("\nClient has been disconnected!\n");
        close(newAddr->sock);
    }
    else
    {
        printf("User [%s] is connected!\n", usernamerecv);
        strcpy(newAddr->username, usernamerecv);
    }
    printf("Connection accepted with client username: %s", newAddr->username);
    //Add information's data to database
    insertFirst(newAddr);
    sem_post(&bin_sem);

    // while (i > -1)
    //         { // 10 second countdown
    //             printf("\033[A\33[2K\t Thread will exit after %d...\n", i);
    //             i--;
    //             sleep(1);
    //         }
    //
    pthread_exit("\r\n--Completed--\r\n");
}

int main(int argc, char **argv)
{

    char sendbuff[256];
    int sockfd, ret, port, res, i = 3;
    void *thread_result;

    connection_t serverAddr;

    pthread_t thread_waitusername, thread;
    int newSocket;
    res = sem_init(&bin_sem, 0, 0);
    if (res != 0)
    {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }
    socklen_t addr_size;

    char buffer[1024];
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        return -1;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[-]perror in connection.\n");
        exit(1);
    }
    printf("[+]Server Socket is created.\n");
    if (sscanf(argv[1], "%d", &port) <= 0)
    {
        fprintf(stderr, "%s: perror: wrong parameter: port\n", argv[0]);
        return -2;
    }
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.address_client.sin_family = AF_INET;
    serverAddr.address_client.sin_port = htons(port);
    serverAddr.address_client.sin_addr.s_addr = INADDR_ANY;

    ret = bind(sockfd, (struct sockaddr *)&serverAddr.address_client, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        printf("[-]perror in binding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d\n", port);

    if (listen(sockfd, 5) == 0)
    {
        printf("[+]Listening....\n");
    }
    else
    {
        printf("[-]perror in binding.\n");
    }

    while (1)
    {
        connection_t *newAddr = (connection_t *)malloc(sizeof(connection_t));
        newSocket = accept(sockfd, (struct sockaddr *)&newAddr->address_client, (socklen_t *)&addr_size);
        if (newSocket < 0)
        {
            printf("Connection has been disconnected!\n");
            free(newAddr);
            exit(1);
        }
        else
        {
            /*waiting for client sent username
                threads handle to receive username*/
            printf("Create thread waits username from client!\n");
            newAddr->sock = newSocket;
            printf("newAddr->sock = %d \n", newAddr->sock);
            res = pthread_create(&thread_waitusername, NULL, waitClientAddusername, (void *)newAddr);
            if (res != 0)
            {
                perror("Thread creation failed");
                exit(EXIT_FAILURE);
            }
            else
                printf("create Ok!\n");
            sem_wait(&bin_sem);
            res = pthread_join(thread_waitusername, &thread_result);
            if (res != 0)
            {
                perror("Thread join failed");
                free(newAddr);
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("\nThread username responds: %s", (char *)thread_result);
                free(newAddr);
            }
            sem_destroy(&bin_sem);
            struct Node *thisClient = head;
            while (thisClient != NULL)
            {
                res = pthread_create(&thread, NULL, roomChat, (void *)thisClient);
                if (res != 0)
                {
                    perror("Thread join failed");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    printf("Thread returned %s", (char *)thread_result);
                }
                break;

                thisClient = thisClient->next;
            }
            printList();
            printf("Waiting for new client connect ...\n");
        }
    }

    return 0;
}

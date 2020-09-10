//For the final example, server5.c, you’ll include the sys/time.h and sys/ioctl.h headers
// instead of signal.h as in the last program, and declare some extra variables to deal with
// select:
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/in.h>
#include <string.h>
#include <errno.h>
//ps -e -T | grep server to see how many process is available
//Implement Code ,can use this function in socketUser.h,(C++)
typedef struct
{
    int sock;
    bool registration;
    char username[30];
    struct sockaddr_in address_client;
    socklen_t socksize;
} connection_t;
struct Node
{
    connection_t client;
    struct Node *next;
};
//number of socket joining.
//Linked list
struct Node *head = NULL;
struct Node *current = NULL;

//Optional///
char *get_IP(char *data);
char *get_DATA(char *data);
/////////
void deleteSocket(struct Node *socket);
void deleteAllist();
void printList();
bool isEmpty();
int lengthOfListSocket();
void insertFirst(connection_t *client);
void insertEnd(connection_t *client);
void delete_from_end();
void delete_from_begin();
struct Node *findClient(int socket);

//Implement Code ,can use this function in socketUser.cpp,(C++)

struct Node *findClient(int socket)
{
    struct Node *ptr;
    ptr = head;
    if (ptr == NULL)
    {
        printf("Linked list of socket is empty.\n");
        return NULL;
    }
    //start from the beginning
    while (ptr != NULL)
    {
        if (ptr->client.sock == socket)
            return ptr;
        ptr = ptr->next;
    }
}
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
// Delete a socket with un-known position
void deleteSocket(struct Node *Socket)
{
    struct Node *list, *_NEXT, *_CONNECT;
    list = head;
    while (list != NULL)
    {
        if (head == Socket)
        {
            delete_from_begin();
            //because just only one socket  deleteSocket(Socket);
        }
        _NEXT = list->next;
        if (_NEXT == NULL)
        {
            if (list == Socket)
            {
                delete_from_end();
            }
            break;
        }

        if (_NEXT == Socket)
        {
            _CONNECT = _NEXT->next;
            list->next = _CONNECT;
            free(_NEXT);
        }
        else
        {
            list = _NEXT;
        }
    }
}
//Free resource:
void deleteAllist()
{
    struct Node *list;
    while (head != NULL)
    {
        list = head;
        head = list->next;
        free(list);
    }
}

//Display the list
void printList()
{
    int num = lengthOfListSocket();
    struct Node *ptr;
    ptr = head;
    if (ptr == NULL)
    {
        printf("Linked list of socket is empty.\n");
        return;
    }
    printf("There are %d users in linked list.\n", num);
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
int lengthOfListSocket()
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
void insertEnd(connection_t *client)
{
    struct Node *t, *temp;
    t = (struct Node *)malloc(sizeof(struct Node));
    t->client = *client;

    if (head == NULL)
    {
        head = t;
        head->next = NULL;
        return;
    }

    temp = head;

    while (temp->next != NULL)
        temp = temp->next;

    temp->next = t;
    t->next = NULL;
}
void delete_from_end()
{
    struct Node *t;
    struct Node *u = NULL;
    int n;

    if (head == NULL)
    {
        printf("Linked list is empty.\n");
        return;
    }
    if (head->next == NULL)
    {
        n = head->client.sock;
        free(head);
        head = NULL;
        return;
    }

    t = head;

    while (t->next != NULL)
    {
        u = t;
        t = t->next;
    }
    n = t->client.sock;
    u->next = NULL;
    free(t);
}
void delete_from_begin()
{
    struct Node *t;
    int n;

    if (head == NULL)
    {
        printf("Linked list is empty.\n");
        return;
    }

    n = head->client.sock;
    t = head->next;
    free(head);
    head = t;
}
int main(int argc, char **argv)
{
    int server_sockfd, client_sockfd;
    int server_len, client_len, port;
    struct sockaddr_in server_address;
    int result;
    fd_set readfds, testfds;
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        return -1;
    }
    if (sscanf(argv[1], "%d", &port) <= 0)
    {
        fprintf(stderr, "%s: perror: wrong parameter: port\n", argv[0]);
        return -2;
    }
    //Create and name a socket for the server:
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    // Create a connection queue and initialize readfds to handle input from server_sockfd:
    listen(server_sockfd, 5);
    FD_ZERO(&readfds);
    // we will set server_ to read ...storage???[t]
    FD_SET(server_sockfd, &readfds);
    // Now wait for clients and requests. Because you have passed a null pointer as the timeout
    // parameter, no timeout will occur. The program will exit and report an error if select returns a
    // value less than 1:
    while (1)
    {
        char recvChar[1024] = {0};
        char userDATA[1024] = {0};
        char usernamerecv[30], ch;
        int fd;
        int nread, n;
        testfds = readfds;
        printf("Server is waiting:\n");
        // FD_SETSIZE , how many sockets is connect,is include server socket.
        result = select(FD_SETSIZE, &testfds, (fd_set *)0,
                        (fd_set *)0, (struct timeval *)0);
        if (result < 1)
        {
            perror("server5");
            exit(1);
        }
        // Once know you’ve got activity, you can find which descriptor it’s on by checking each in
        // turn using FD_ISSET:
        for (fd = 0; fd < FD_SETSIZE; fd++)
        {
            // printf("fd:%d\n", fd); //try to use another type instead of use FD_SETSIZE because it measures about 1024, use another to feel different
            if (FD_ISSET(fd, &testfds))
            {
                // If the activity is on server_sockfd, it must be a request for a new connection, and you add the
                // associated client_sockfd to the descriptor set:
                if (fd == server_sockfd)
                {
                    connection_t *new_Client = (connection_t *)malloc(sizeof(connection_t));
                    client_sockfd = accept(server_sockfd, (struct sockaddr *)&new_Client->address_client, (socklen_t *)&new_Client->socksize);
                    if (client_sockfd < 0)
                    {
                        printf("Connection has been disconnected!\n");
                        free(new_Client);
                        exit(1);
                    }
                    printf("==>Socket: [%d] connect \n", client_sockfd);
                    new_Client->sock = client_sockfd;
                    // this's a first time client participate this room.
                    new_Client->registration = false;
                    insertFirst(new_Client); // If we storage client on linked list.Use this case
                    FD_SET(new_Client->sock, &readfds);
                }
                // If it isn’t the server, it must be client activity. If close is received, the client has gone away, and
                // you remove it from the descriptor set. Otherwise, you "serve" the client as in the previous
                // examples.
                else
                {
                    // search Id on Data,if not,request to client to sign up before go to chat box.
                    struct Node *eventConnect = findClient(fd);
                    // The room chat have 1024 client ,because we have FD_SETSIZE =1024;
                    if (eventConnect->client.registration == false)
                    {
                        memset(usernamerecv, 0, 30);
                        n = read(eventConnect->client.sock, usernamerecv, 30);
                        if (n < 0)
                        {
                            perror("Error reading from client ");
                        }
                        else if (n == 0)
                        {
                            printf("\nClient has been disconnected!\n");
                            FD_CLR(eventConnect->client.sock, &readfds);
                            deleteSocket(eventConnect);
                        }
                        else
                        {
                            printf("User [%s] is connected!\n", usernamerecv);
                            strcpy(eventConnect->client.username, usernamerecv);
                            //regis_ client OK...
                            eventConnect->client.registration = true;
                        }
                    }
                    else
                    {
                        /* code */
                        memset(recvChar, 0, 30);
                        n = read(eventConnect->client.sock, recvChar, 1024);
                        if (n < 0)
                        {
                            perror("Error reading from client ");
                        }
                        else if (n == 0)
                        {
                            printf("\nClient [%s] has been disconnected!\n", eventConnect->client.username);
                            printf("Delete Client :[%s]\n", eventConnect->client.username);
                            FD_CLR(eventConnect->client.sock, &readfds);
                            deleteSocket(eventConnect);
                            printf("Number of alive user in this room: %d\n", lengthOfListSocket());
                        }
                        else
                        {
                            /* code */
                            const char *clientSay = " said:";
                            strcat(userDATA, eventConnect->client.username);
                            strcat(userDATA, clientSay);
                            strcat(userDATA, recvChar);
                            struct Node *sendClient = head;
                            while (sendClient != NULL)
                            {
                                if (sendClient != eventConnect)
                                {
                                    if (write(sendClient->client.sock, userDATA, sizeof(userDATA)) > 0)
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
                        memset(recvChar, 0, 1024);
                        memset(userDATA, 0, 30);
                    }
                }
            }
        }
    }
}
// void FD_ZERO(fd_set *fdset);
// void FD_CLR(int fd, fd_set *fdset);
// void FD_SET(int fd, fd_set *fdset);
// int FD_ISSET(int fd, fd_set *fdset);
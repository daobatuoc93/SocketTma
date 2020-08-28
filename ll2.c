
#include <stdio.h>
#include <stdlib.h>
struct node {
    int data;
    struct node* next;
};
struct node* start = NULL;
typedef struct
{
    int sock;
    char username[30];
}connection_t;
struct Node
{
    connection_t client;
    struct Node* next;
};

struct Node* head = NULL;
void insert_at_begin(connection_t *x);
void insert_at_end(int);
void traverse();
void deleteAllist();
void delete_from_begin();
void delete_from_end();
void deleteAllistpid2();
int count = 0;

int main() {
    int i;
    char un[30];
    memset(un, 0, 30);
    connection_t *cl=(connection_t*)malloc(sizeof(connection_t));

    for (;;) {
        printf("1. Insert an element at the beginning of linked list.\n");
        printf("2. Insert an element at the end of linked list.\n");
        printf("3. Traverse linked list.\n");
        printf("4. Delete an element from beginning.\n");
        printf("5. Delete an element from end.\n");
        printf("6. Delete all list.\n");
        printf("8. Delete any elements.\n");
        printf("7. Exit\n");
        scanf("%d", &i);
        if (i == 1) {
            printf("Enter value of element\n");
            cl->sock = 1;
            strcpy(cl->username, "Ali");
            insert_at_begin(cl);
        }
        else if (i == 2) {
            printf("Enter value of element\n");
            cl->sock = 2;
            strcpy(cl->username, "asdw");
            insert_at_begin(cl);
        }
        else if (i == 9) {
            printf("Enter value of element\n");
            cl->sock = 9;
            strcpy(cl->username, "a124sd");
            insert_at_begin(cl);
        }
        else if (i == 10) {
            printf("Enter value of element\n");
            cl->sock = 10;
            strcpy(cl->username, "asdfsa");
            insert_at_begin(cl);
        }
        else if (i == 3)
            traverse();
        else if (i == 4)
            delete_from_begin();
        else if (i == 5)
            delete_from_end();
        else if (i == 6)
            deleteAllist();
        else if (i == 8)
            deleteAllistpid2();
        else if (i == 7)
            break;
        else
            printf("Please enter valid input.\n");
    }
    free(cl);
    return 0;
}

void insert_at_begin(connection_t *x) {
    struct Node* t;

    t = (struct Node*)malloc(sizeof(struct Node));
    t->client = *x;
    count++;

    if (head == NULL) {
        head = t;
        head->next = NULL;
        return;
    }

    t->next = head;
    head = t;
}

void insert_at_end(int x) {
    struct node* t, * temp;

    t = (struct node*)malloc(sizeof(struct node));
    t->data = x;
    count++;

    if (start == NULL) {
        start = t;
        start->next = NULL;
        return;
    }

    temp = start;

    while (temp->next != NULL)
        temp = temp->next;

    temp->next = t;
    t->next = NULL;
}
void deleteAllist(){
    struct Node *list;   
    while (head != NULL)
    {
        list = head;
        head = list->next;
        free(list);        
    }
}
void deleteAllistpid2() {
    struct Node* list,*_NEXT,*_CONNECT;
    list = head;    
    while (list != NULL)
    {   
        if (head->client.sock == 2) {
            delete_from_begin();
            deleteAllistpid2();
        }
        _NEXT = list->next;
        if (_NEXT == NULL)
        {
            if (list->client.sock == 2) {
                count--;
                delete_from_end();
            }
            break;
        }
        
        if (_NEXT->client.sock == 2)
        {   
            
            _CONNECT = _NEXT->next;
            list->next = _CONNECT;
            count--;
            free(_NEXT);
        }
        else
        {
            list = _NEXT;
        }        
    }
}
void traverse() {
    struct Node* t;
    t = head;
    if (t == NULL) {
        printf("Linked list is empty.\n");
        return;
    }

    printf("There are %d elements in linked list.\n", count);

    while (t != NULL) {
        printf("sock %d\n", t->client.sock);
        printf("username %d\n", t->client.username);
        t = t->next;
    }
   
}

void delete_from_begin() {
    struct Node* t;
    int n;

    if (head == NULL) {
        printf("Linked list is empty.\n");
        return;
    }

    n = head->client.sock;
    t = head->next;
    free(head);
    head = t;
    count--;

    printf("%d deleted from the beginning successfully.\n", n);
}

void delete_from_end() {
    struct Node* t;
    struct Node *u = NULL;
    int n;

    if (head == NULL) {
        printf("Linked list is empty.\n");
        return;
    }

    count--;

    if (head->next == NULL) {
        n = head->client.sock;
        free(head);
        head = NULL;
        printf("The socket %d deleted from end successfully.\n", n);
        return;
    }

    t = head;

    while (t->next != NULL) {
        u = t;
        t = t->next;
    }
    n = t->client.sock;
    u->next = NULL;
    free(t);
    printf("The socket %d deleted from end successfully.\n", n);
}


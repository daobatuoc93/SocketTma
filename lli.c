#include<stdio.h>
#include<stdlib.h>
struct data{
  int idata;
  char name[110];
}
struct node
{
    struct data *newdata;
    struct node *next;
};
struct node *head = NULL;
void addFirst( struct data *newData)
{
      //create a new node
      struct node *newNode = malloc(sizeof(struct node));
      newNode->newdata->idata = newData->idata;
	  newNode->newdata->name =newData->name;
      newNode->next = head;

      head = newNode;
      
}

void printList()
{
    struct node *temp = head;

    //iterate the entire linked list and print the data
    while(temp != NULL)
    {
         printf("[%d] ,[%s] ->", temp->newdata->idata,temp->newdata->name);
         temp = temp->next;
    }
    printf("NULL\n");
}

int main()
{
	struct data *newdata;
     newdata->idata = 10;
	newdata->name="hekk";
    addFirst(newdata);
	 newdata->idata = 20;
	newdata->name="3rhekk";
    addFirst(newdata);
     return 0;
}
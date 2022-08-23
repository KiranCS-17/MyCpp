#include<stdio.h>
#include<stdlib.h>

struct Node
{
  int data;
  struct Node* prev;
  struct Node* next;
};

struct Node *head,*currentNode;

void setCurrentNode(struct Node* nextNode)
{
  currentNode = nextNode;
}

struct Node* getCurrentNode()
{
  return currentNode;
}

void initilizeList()
{
  head = (struct Node*)malloc(sizeof(struct Node));
  head->data = -1; // head
  head->prev = NULL; // head
  head->next = NULL; // head
  setCurrentNode(head);
}


void insert(int value)
{
  struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
  struct Node* currentNode = getCurrentNode();
  currentNode->next = newNode;
  newNode->prev = currentNode;
  newNode->data = value;
  newNode->next = NULL;
  setCurrentNode(newNode);
}

struct Node* search(int value)
{
    struct Node* latest = getCurrentNode();
    while(latest != NULL)
  	{
  		if(latest->data != -1 && latest->data == value )
  		{
  	    	return latest;
  		}
  		latest = latest->prev;
  	}

  	return NULL;

}

void deleteNode(int value)
{
	struct Node* nodeWithVal = search(value);
	if(NULL != nodeWithVal)
	{
		if(nodeWithVal->prev != NULL)
		{
			struct Node* nodeAfter = nodeWithVal->next;
			struct Node* nodeBefore = nodeWithVal->prev;
			nodeBefore->next = nodeAfter;
			if(nodeAfter != NULL)
			{
				nodeAfter->prev = nodeBefore;
		    }
		    else
		    {
				setCurrentNode(nodeBefore);
			}

		}
		else if(nodeWithVal->next == NULL)
		{
			struct Node* nodeBefore = nodeWithVal->prev;
			nodeBefore->next = NULL;
			setCurrentNode(nodeBefore);

		}

		free(nodeWithVal);
	}
}

void printNode(struct Node* aNode)
{
	if(aNode != NULL)
	{
		if(aNode->prev != NULL)
		{
			 printf("prev-> %p\n",aNode->prev);
		}
		else
		{
			 printf("prev-> NULL\n");
		}
		if(aNode->next != NULL)
		{
			 printf("next-> %p\n",aNode->next);
		}
		else
		{
     		 printf("next-> NULL\n");
		}

		printf("data: %d\n",aNode->data);

	}

}

void printList()
{

	struct Node* currentNode = getCurrentNode();

	while(currentNode != NULL)
	{
		if(currentNode->data != -1)
		{
	    	printf("%d\n",currentNode->data);
		}
		currentNode = currentNode->prev;

	}

	printf("-----------------------------------\n");

}



int main()
{
	initilizeList();

	insert(4);
	insert(5);
	/*insert(6);
	insert(61);
	insert(64);
	insert(67);
	insert(68);
*/
	printList();
	/*printf("%p \n ", head);
	printNode(head);
	printNode(search(4));
	printNode(search(5));
	printNode(search(6));*/

	deleteNode(5);
	printList();
	deleteNode(4);
	printList();
}
#ifndef LIST_H
#define LIST_H

typedef struct LinkedList LinkedList;
typedef struct LinkedListNode LinkedListNode;

LinkedList* 
LinkedList_create();

void 
LinkedList_append( LinkedList* list, char* data );

void
LinkedList_delete( LinkedList* list );

LinkedListNode*
LinkedList_getFirst( LinkedList* list );

LinkedListNode*
LinkedList_getLast( LinkedList* list );

int
LinkedList_getSize(LinkedList* list);

char* LinkedList_getData(LinkedListNode* node);

char* LinkedList_getDataAtPosition(LinkedList* list, int position);
#endif

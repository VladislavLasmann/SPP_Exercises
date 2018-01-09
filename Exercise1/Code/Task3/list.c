#include "list.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * A LinkedListNode represents a list-element. It has following properties:
 * LinkedListNode* previous     A pointer to it's previous element.
 *                              It is NULL, if it's the first element in the list.
 * LinkedListNode* next         A pointer to it's next element.
 *                              It is NULL, if it's the last element in the list.
 * char* data                   A string as key
 */
struct LinkedListNode{
    LinkedListNode* previous;
    LinkedListNode* next;
    char* data;
};

/**
 * A LinkedList represents a doubly linked list of LinkedListNodes and has following properties:
 * LinkedListNode* first        A pointer to the first element in the list.
 *                              It is NULL, when the list is empty.
 * LinkedListNode* last         A pointer to the last element in the list.
 *                              It is NULL, when the list is empty.
 */
struct LinkedList {
    LinkedListNode* first;
    LinkedListNode* last;
};


/**
 * Creates and returns a doubly linked list (LinkedList)
 * @return a pointer to the doubly linked list (LinkedList)
 */
LinkedList* LinkedList_create(){

    LinkedList* theList = malloc(sizeof(LinkedList));
    theList->first = NULL;
    theList->last = NULL;

    return theList;
}

/**
 * Creates an LinkedListElement with the given parameter data and appends it
 * to end of the given LinkedList.
 * @param list a pointer to an correctly built LinkedList, where the element will be appended to
 * @param data a non-empty String
 */
void LinkedList_append( LinkedList* list, char* data){

    // Creating the node
    LinkedListNode* theNode = malloc( sizeof(LinkedListNode) );
    theNode->data = strdup( data );

    // append to the last node
    theNode->previous = list->last;
    theNode->next = NULL;

    // append last node to theNode
    if( list->last != NULL )
        list->last->next = theNode;

    // now theNode is last node in list
    list->last = theNode;

    // if first one null, insert in empty list
    if( list->first == NULL )
        list->first = theNode;
}

/**
 * Destroys an given LinkedList. Frees every data in the list.
 * @param list a pointer to an correcty built LinkedList
 */
void LinkedList_delete( LinkedList* list){
    //printf("DEBUG_delete: assign node\n");
    LinkedListNode* node = LinkedList_getFirst(list);

    //printf("DEBUG_delete: entering loop\n");
    while( node != NULL ){
        //printf("DEBUG_delete: nextNode == node->next \n");
        // lifetime of that node will and after this loop ends
        LinkedListNode* nextNode = node->next;
        //printf("DEBUG_delete: free node\n");
        free( node );
        //printf("DEBUG_delete: node = nextNode\n");
        // last nextNode is NULL, no free required
        node = nextNode;
    }
    // free that list
    //printf("free list\n");

    free( list );
    list = NULL;
    //if (list == NULL) {
    //    printf("list is null\n");
    //}
}

/**
 * Returns the first element in a LinkedList. NULL, if the list is empty.
 * @param list a pointer to an correctly built LinkedList
 * @return the first element in a LinkedList or NULL, if the list is empty
 */
LinkedListNode* LinkedList_getFirst( LinkedList* list){
    return list->first;
}

/**
 * Returns the last element in a LinkedList. NULL, if the list is empty.
 * @param list a pointer to an correctly built LinkedList
 * @return the last element in a LinkedList or NULL, if the list is empty
 */
LinkedListNode* LinkedList_getLast( LinkedList* list ){
    return list->last;
}


/**
 * for debugging:
 *
 * int LinkedList_getSize( LinkedList* list)
 * char* LinkedList_getData( LinkedListNode* node)
 */

int LinkedList_getSize( LinkedList* list ){
    int size = 0;
    LinkedListNode* node = LinkedList_getFirst( list );
    while( node != NULL ){
        size++;
        node = node->next;
    }

    return size;
}

char* LinkedList_getData( LinkedListNode* node ){
    return node->data;
}

char* LinkedList_getDataAtPosition( LinkedList* list, int position ){
    LinkedListNode* node = list->first;
    for( int i = 0; i < position; i++){
        if( node->next == NULL)
            return NULL;
        else
            node = node->next;
    }
    return node->data;
}



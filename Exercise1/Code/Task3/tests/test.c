#include <stdio.h>
#include <stddef.h>     // for NULL
#include <string.h>
#include"../list.h"

int main(){

    int AMOUNT_INPUT_STRINGS = 5;
    char names[AMOUNT_INPUT_STRINGS][20];

    printf("Enter %d dates\n", AMOUNT_INPUT_STRINGS );
    for(int i = 0; i < AMOUNT_INPUT_STRINGS; i++){
        printf("Enter data#%d: ", i);
        scanf("%s", names[i]);
    }
    printf("Input done\n");

    printf("Creating nodes by appending\n");
    LinkedList* list = LinkedList_create();
    for (int i = 0; i < AMOUNT_INPUT_STRINGS; ++i) {
        LinkedList_append(list, names[i]);
    }
    if( LinkedList_getSize(list) == AMOUNT_INPUT_STRINGS )
        printf("LinkedList_append() correct by list size\n");
    else
        printf("LinkedList_append() failed by list size\n");


    printf("Checking function LinkedList_append() by content\n");
    for(int i = 0; i < AMOUNT_INPUT_STRINGS; i++){
        if( strcmp( LinkedList_getDataAtPosition(list, i), names[i] ) != 0 )
            printf("Error: LinkedList_append() failed at position %d, expected: %s, but got %s\n", i, names[i], LinkedList_getDataAtPosition(list, i) );
    }
    printf("If no errors occurred, LinkedList_append() works correctly\n");

    if (list != NULL)
        printf("Good: list != null before deleting\n");
    else
        printf("Error: list == null before deleting\n" );
    printf("Deleting list...\n");

    LinkedList_delete( list );
    printf("Deleting done....\n");
}

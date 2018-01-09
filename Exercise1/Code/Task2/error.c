#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

typedef struct  {
    char* street;
    int number;
    int post_code;
    char* city;
} adress;

adress* create_adress( char* street, int number, int post_code, char* city) {
    // printf("enter create_adress\n");

    adress* new_adress = malloc(sizeof(adress) );                          //*2*

    new_adress->street      = street;
    new_adress->number      = number;
    new_adress->post_code   = post_code;
    new_adress->city        = city;

    // printf("exit create_adress\n");
    return new_adress;
}

adress* duplicate_adress (adress orig) {    // *3*
    return create_adress( strdup(orig.street), orig.number, orig.post_code, strdup(orig.city) );
}


/*
void printAdress( adress* addr ){
    printf("=======================\n");
    printf("Entered Adress-Print\n");
    printf("Street: %s\n", addr->street);
    printf("Number: %d\n", addr->number);
    printf("PostCode: %d\n", addr->post_code);
    printf("City: %s\n", addr->city);
    printf("=======================\n");
}


int main() {
    printf("before a1\n");
    adress* a1 = create_adress("Mornewegstr.", 30, 64293, "Darmstadt");
    printf("after a1\n");
    printf("before a2\n");
    adress* a2 = duplicate_adress( *a1 );   // *1*
    printf("after a2\n");

    printAdress(a1);
    free( a1 );
    printAdress(a2);
    free( a2 );
}
*/
/*
 * Errors:
 * No. 1:   *1*      Compiler-error: the parameter of duplicate_adress and the passed object are not the same type
 *                      - duplicate_adress accepts parameter of type adress
 *                      - but was passed an **adress (pointer of an adress-pointer)
 *
 * No. 2:   *2*     Runtime-error: No memory for an adress object allocated. Pointer doesnt point on an adress.
 *                      - The creation of a pointer does not allocate memory for a value (02_SPP_C.pdf, p. 86)
 *
 * No. 3:   *3*     Runtime-error: adress-object was not duplicated, but the reference to the object.
 *                      - only the a pointer to the reference was returned, but the copied reference doesnt exist after function call
 *                      - not persistant memory (02_SPP_C.pdf, p. 99)
 *
 */
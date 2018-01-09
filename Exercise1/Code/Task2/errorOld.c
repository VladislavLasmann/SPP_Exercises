#include <stdlib.h>

typedef struct
{
    char* street;
    int number;
    int post_code;
    char* city;
} adress;

adress*
create_adress( char* street,
    int number,
    int post_code,
    char* city)
{
    adress* new_adress;                     // *2*
    new_adress->street      = street;
    new_adress->number      = number;
    new_adress->post_code   = post_code;
    new_adress->city        = city;
    return new_adress;
}

adress*
duplicate_adress (adress orig)
{
    adress new_adress = orig;               // *3*
    return &new_adress;
}

int main()
{
    adress* a1 = create_adress("Mornewegstr.", 30, 64293, "Darmstadt");
    adress* a2 = duplicate_adress( &a1 );   // *1*
    free( a1 );
    free( a2 );
}

/*
 * Errors:
 * No. 1:   *1*      Compiler-error: the parameter of duplicate_adress and the passed object are not the same type
 *                      - need to use an *(dereference-operator) instead of an & (reference-operator)
 *
 * No. 2:   *2*     Runtime-error: No memory for an adress object allocated. Pointer doesnt point on an adress.
 *                      - The creation of a pointer does not allocate memory for a value (02_SPP_C.pdf, p. 86)
 *                      - Thus the commands in line 18 - 21 are not legal
 *
 * No. 3:   *3*     Runtime-error: adress-object was not duplicated, but the reference to the object.
 *                      - only the a pointer to the reference was returned, but the copied reference doesnt exist after function call
 *                      - not persistant memory (02_SPP_C.pdf, p. 99)
 *
 */
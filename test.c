#include <stdio.h>
#include "darray.h"

typedef struct testing{
    char* test;
}testing;

void testing_free(testing* test){
    free(test->test);
}

DYNAMIC_ARRAY(int, normal_int_array, DARRAY_MC, DARRAY_GR)
DYNAMIC_ARRAY_MULTILEVEL(testing, testing_array, testing_free, DARRAY_MC, DARRAY_GR)
// DYNAMIC_ARRAY_DECL(int, split_int_array);
// DYNAMIC_ARRAY_IMPL(int, split_int_array);



int main(){
    printf(\
        "THIS IS A TEST TO SEE IF IT COMPILES CAUSE \
        FINDING MISSING SEMI COLONS IN A MACRO IS A MASSIVE \
        PAIN IN THE ASS\n" \
    );
}
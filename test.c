#include <stdio.h>
#include "darray.h"

DYNAMIC_ARRAY(int, normal_int_array)
DYNAMIC_ARRAY_DECL(int, split_int_array);
DYNAMIC_ARRAY_IMPL(int, split_int_array);



int main(){
    printf(\
        "THIS IS A TEST TO SEE IF IT COMPILES CAUSE \
        FINDING MISSING SEMI COLONS IN A MACRO IS A MASSIVE \
        PAIN IN THE ASS\n" \
    );
}
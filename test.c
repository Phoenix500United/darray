#include "darray.h"

DYNAMIC_ARRAY_DECL(int, int_array)
DYNAMIC_ARRAY_IMPL(int, int_array)

int main(){
    printf(\
        "THIS IS A TEST TO SEE IF IT COMPILES CAUSE \
        FINDING MISSING SEMI COLONS IN A MACRO IS A MASSIVE \
        PAIN IN THE ASS\n" \
    );
}
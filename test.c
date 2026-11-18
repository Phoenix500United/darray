#include <stdio.h>
#include "darray.h"
#include "assert.h"

DYNAMIC_ARRAY(int, int_array, DARRAY_MC, DARRAY_GR)
DYNAMIC_ARRAY_MULTILEVEL(int_array, array_of_int_arrays, int_array_free, DARRAY_MC, DARRAY_GR)



// DYNAMIC_ARRAY_DECL(int, split_int_array);
// DYNAMIC_ARRAY_IMPL(int, split_int_array);



int main(){
    int_array array; 
    int b[4] = {0, 1, 2, 3};
    if(int_array_init(&array, 4, b) == false){
        return 0;
    }
    
    for(size_t i = 0; i < array.size; ++i){
        assert(array.data[i] == b[i]);
    }

    if(int_array_reinit(&array, 4, b) == false){
        return 0;
    }
    
    for(size_t i = 0; i < array.size; ++i){
        assert(array.data[i] == b[i]);
    }
    
    

    int_array_reserve_exact(&array, 15);
    assert(array.capacity >= 15);
    if(!int_array_recalculate_capacity(&array)){
        return 0;
    };

    printf("SIZE: %ld, CAPACITY: %ld\n", array.size, array.capacity);
    assert(array.capacity == DARRAY_MC);

    int_array_push(&array, 4);
    assert(array.data[array.size - 1] == 4);

    int_array_c_array_append(&array, 4, b);
    printf("SIZE: %ld, CAPACITY: %ld\n", array.size, array.capacity);

    for(size_t i = 5; i < array.size; ++i ){
        assert(array.data[i] == b[i - 5]);
    }

    int_array array2;
    int_array_homogenous_init(&array2, 15, 2);
    for(size_t i = 0; i < array2.size; ++i){
        assert(array2.data[i] == 2);
    }
    int_array_homogenous_reinit(&array2, 15, 3);
    for(size_t i = 0; i < array2.size; ++i){
        assert(array2.data[i] == 3);
    }

    int_array_insert(&array2, 3, 7);
    assert(array2.data[3] == 7);
    int_array_recalculate_capacity(&array2);
    int_array_erase(&array2, 3);
    for(size_t i = 0; i < array2.size; ++i){
        assert(array2.data[i] == 3);
    }

    int_array_recalculate_capacity(&array);
    int_array_erase_range(&array, 0, 4);
    assert(array.data[0] == 4);

    int_array_free(&array2);
    int_array_free(&array);
}
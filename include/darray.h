
#ifndef DARRAY_H
#define DARRAY_H
//README
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 
// GIGA IMPORTANT NOTE!!!
// the \ character means "continue macro onto next line"
// it's very very important that this is the last character on the line!
// not even a space or a comment can come after, if it does, you will get giga errors!
// 
// MEDIUM IMPORTANT NOTE!!
// so the current implementation isn't toooooo safe, since realloc can fail silently inside the macro
// gotta add some extra checks in there to make sure if realloc fails then it panics properly
// for example overflow can happen and we allocate 17b instead of 12Gb 
// or if we ask for more memory than can be given to us, realloc will fail
// these failures are fine, the problem is right now they will happen silently, making bugs hard to track
// at some point, this will have to be made safer, which is a performance hit, but worth it for air tightness
// 
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#define DYNAMIC_ARRAY(type, typename)									 	\
	typedef struct { type *data; size_t size; size_t capacity;}typename;	\
																			\
	void typename##_init(typename *arr, size_t size, type* c_arr){			\
		if (c_arr == NULL){													\
			arr->data = NULL;												\
			arr->size = 0;													\
			arr->capacity = 0;												\
		}else{																\
			arr->size = size;												\
			arr->capacity = size;											\
			arr->data = (type*)malloc(arr->capacity * sizeof(type));		\
			for(size_t i = 0; i < size; ++i){								\
				arr->data[i] = c_arr[i];									\
			}																\
		}																	\
	}																		\
	void typename##_push(typename *arr, type value){						\
		if(arr->size >= arr->capacity){										\
			arr->capacity = arr->capacity ? arr->capacity * 2 : 8;			\
			arr->data = realloc(arr->data, arr->capacity);					\
		}																	\
		arr->data[arr->size++] = value;										\
	}																		\
	type typename##_pop(typename *arr){										\
		return arr->data[--arr->size];										\
	}																		\
	void typename##_recalculate_capacity(typename *arr){					\
		arr->capacity = arr->size;											\
		arr->data = realloc(arr->data, arr->capacity * sizeof(type));		\
	}																		\
	void typename##_clear(typename *arr){									\
		arr->size = 0;														\
	}																		\
	void typename##_reset(typename *arr){									\
		arr->size = 0;														\
		arr->capacity = 0;													\
		free(arr->data);													\
		arr->data = NULL;													\
	}																		\
	void typename##_free(typename *arr){									\
		free(arr->data);													\
	}																		\
	void typename##_reserve(typename *arr, size_t newcapacity){				\
		if(arr->capacity == 0 && newcapacity > 0){							\
			arr->capacity = newcapacity;									\
			arr->data = (type*)malloc(arr->capacity * sizeof(type));		\
		}																	\
																			\
		else if(newcapacity > arr->capacity){								\
			arr->capacity = newcapacity;									\
			arr->data = realloc(arr->data, arr->capacity * sizeof(type));	\
		}																	\
	}																		\
																			\
	void typename##_insert(typename *arr, type value, size_t index){		\
		if(arr->size == arr->capacity){										\
			arr->capacity = arr->capacity ? arr->capacity*2 : 8;			\
			arr->data = realloc(arr->data, arr->capacity * sizeof(type)); 	\
		}																	\
		for (size_t i = arr->size; i > index; i--){							\
			arr->data[i] = arr->data[i-1];									\
		}																	\
		arr->data[index] = value;											\
		arr->size++;														\
	}																		\
	void typename##_erase(typename *arr, size_t index){						\
		for(size_t i = index; i < arr->size - 1; ++i){						\
			arr->data[i] = arr->data[i + 1];								\
		}																	\
		--arr->size;														\
	}																		\
	void typename##_erase_range(typename *arr, size_t rangestart, size_t rangeend){	\
		if (rangestart > rangeend){												 	\
			return;																	\
		}																			\
		for(size_t i = rangestart; i < rangeend && rangeend + i < arr->size; i++){	\
			arr->data[i + rangestart] = arr->data[i + rangeend];					\
		}																			\
		arr->size -= rangeend - rangestart; 										\
	}																				\
	void typename##_array_append(typename *arr1, typename *arr2){					\
		size_t newsize = arr1->size + arr2->size;									\
		if(arr1->capacity < newsize){												\
			arr1->capacity = arr1->size <= arr2->size ? (arr2->size ? 2*arr2->size : 8) : 2*arr1->size; \
			arr1->data = realloc(arr1->data, arr1->capacity * sizeof(*(arr1->data)));\
		}																			\
		for(size_t i = 0; i < arr2->size; ++i){										\
			arr1->data[arr1->size+i] = arr2->data[i];									\
		}																			\
		arr1->size = newsize;														\
	}																				\

//==================================================================================================================================
// MULTI LEVEL FOR WHEN THE TYPE CONTAINED IN THE ARRAY CONTAINS A HEAP POINTER AND REQUIRES EXTRA FREEING TO ENSURE NO MEM LEAKS 
//==================================================================================================================================

#define DYNAMIC_ARRAY_MULTILEVEL(type, typename)							\
	typedef struct { type *data; size_t size; size_t capacity;} typename;	\
																			\
	void typename##_init(typename *arr, size_t size, type* c_arr){			\
		if (c_arr == NULL){													\
			arr->data = NULL;												\
			arr->size = 0;													\
			arr->capacity = 0;												\
		}else{																\
			arr->size = size;												\
			arr->capacity = size;											\
			arr->data = (type*)malloc(arr->capacity * sizeof(type));		\
			for(size_t i = 0; i < size; ++i){								\
				arr->data[i] = c_arr[i];									\
			}																\
		}																	\
	}																		\
	void typename##_push(typename *arr, type value){						\
		if(arr->size >= arr->capacity){										\
			arr->capacity = arr->capacity ? arr->capacity * 2 : 8;			\
			arr->data = realloc(arr->data, arr->capacity);					\
		}																	\
		arr->data[arr->size++] = value;										\
	}																		\
	type typename##_pop(typename *arr){										\
		type tmp = arr->data[--arr->size];									\
		type##_free(&arr->data[arr->size]);									\
		return tmp; 														\
	}																		\
	void typename##_recalculate_capacity(typename *arr){					\
		arr->capacity = arr->size;											\
		arr->data = realloc(arr->data, arr->capacity * sizeof(type));		\
	}																		\
	void typename##_clear(typename *arr){									\
		for(size_t i = 0; i < arr->size; ++i){								\
			type##_free(&arr->data[i]); 									\
		}																	\
		arr->size = 0;														\
	}																		\
	void typename##_reset(typename *arr){									\
		arr->size = 0;														\
		arr->capacity = 0;													\
		for(size_t i = 0; i < arr->size; ++i){								\
			type##_free(&arr->data[i]);										\
		}																	\
		free(arr->data);													\
		arr->data = NULL;													\
	}																		\
	void typename##_free(typename *arr){									\
		for(size_t i = 0; i < arr->size; ++i){								\
			type##_free(&arr->data[i]);										\
		}																	\
		free(arr->data);													\
	}																		\
	void typename##_reserve(typename *arr, size_t newcapacity){				\
		if(newcapacity > arr->capacity){									\
			arr->capacity = newcapacity;									\
			arr->data = realloc(arr->data, arr->capacity * sizeof(type));	\
		}																	\
	}																		\
																			\
	void typename##_insert(typename *arr, type value, size_t index){		\
		if(arr->size == arr->capacity){										\
			arr->capacity = arr->capacity ? arr->capacity*2 : 8;			\
			arr->data = realloc(arr->data, arr->capacity * sizeof(type)); 	\
		}																	\
		for (size_t i = arr->size; i > index; i--){							\
			arr->data[i] = arr->data[i-1];									\
		}																	\
		arr->data[index] = value;											\
		arr->size++;														\
	}																		\
	void typename##_erase(typename *arr, size_t index){						\
		type##_free(&arr->data[index]);										\
		for(size_t i = index; i < arr->size - 1; ++i){						\
			arr->data[i] = arr->data[i + 1];								\
		}																	\
		--arr->size;														\
	}																		\
	void typename##_erase_range(typename *arr, size_t rangestart, size_t rangeend){	\
		if (rangestart > rangeend){												 	\
			return;																	\
		}																			\
		size_t rangedif = rangeend-rangestart;										\
		for(size_t i = rangestart; i < rangeend; ++ i){								\
			type##_free(&arr->data[i]);												\
			arr->data[i] = arr->data[i + rangedif];									\
		}																			\
		for(size_t i = rangeend; i < arr->size; i++){								\
			arr->data[i] = arr->data[i + rangedif];									\
		}																			\
		arr->size -= rangeend - rangestart; 										\
	}																				\
	void typename##_array_append(typename *arr1, typename *arr2){					\
		size_t newsize = arr1->size + arr2->size;									\
		if(arr1->capacity < newsize){												\
			arr1->capacity = arr1->size <= arr2->size ? (arr2->size ? 2*arr2->size : 8) : 2*arr1->size; \
			arr1->data = realloc(arr1->data, arr1->capacity * sizeof(*(arr1->data)));\
		}																			\
		for(size_t i = 0; i < arr2->size; ++i){										\
			arr1->data[arr1->size+i] = arr2->data[i];								\
		}																			\
		arr1->size = newsize;														\
	}																				\










// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// TODO: finish the rest of these macros:
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-




#define DYNAMIC_ARRAY_ARRAY_INSERT(arr1, arr2, index)		
//inserts array 2 into array 1 starting at index so [1 2 3 4], [7,8], index: 2 [1, 2, 7, 8, 3, 4]



#define DYNAMIC_ARRAY_REMOVE_SWAP(arr, index) 
// removes an item from an array, and put the last item of the array in its place
// seems quite strange at first, but this one is actually important because it enables optimization
// with the normal remove, we need to shift everything over by 1 which is not trivial
// with remove_swap, the last item pops off the top and fills in the gap, no shifting required
// does NOT preserve the order



#define DYNAMIC_ARRAY_FILL(arr, value)
// replaces every item of the array with a certain value <<< technically medium tier (see below)

// MEDIUM TIER IDEAS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// sorting (bunch of algorythms)
// reversing
// index of an item
// array contains item?
// MEDIUM NOTE: these are kinda mid ideas because they dont apply specifically to dynamic arrays
// for example sorting an array has nothing to do with growing or shrinking
// sorting can be implimented separately from dynamic arrays, and so can everything else here
// we shouldnt define new stuff for this unless it touches the size or capacity, capiche?
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


// OLD IMPLEMENTATION
/*

#define DYNAMIC_ARRAY_INIT(arr)	\
do {							\
	(arr).data = NULL;			\
	(arr).size = 0;				\
	(arr).capacity = 0;			\
} while(0)
// USAGE:
// DYNAMIC_ARRAY_INIT(xs);
// sets everything in the array to 0 or nullpointer, because we dont want garbage data

// NOTE:
// this macro uses a do{} while() construct so its all considered a single statement by the compiler
// that's important because in usage you want this to behave as a single statement

#define DYNAMIC_ARRAY_PUSH(arr, value)												\
do {																				\
	if ((arr).size >= (arr).capacity) {												\
		(arr).capacity = (arr).capacity ? (arr).capacity * 2 : 8;					\
																					\
		(arr).data = realloc( (arr).data, (arr).capacity * sizeof(*(arr).data) );	\
	}																				\
	(arr).data[(arr).size++] = (value);												\
} while(0)
// USAGE: 
// DYNAMIC_ARRAY_PUSH(xs, 1617149); 
// pushes the value 1617149 onto the xs array, increases size by 1, 
// and increases capacity if capacity is too small
// treats the array like a stack, allows you to push values onto the array

// SUPER NOTE: maybe just think of this one as the ADD function.

// NOTE: if the capacity of the array is too small to fit the new item, it doubles the size
// NOTE: if the capacity is 0, then it hard sets capacity to 8, because 0 * 2 = 0
// NOTE: capacity is different from size! 
//		size     = actual amount of items inside the array
//		capacity = the amount of slots that are allocated to the array, always bigger than size

#define DYNAMIC_ARRAY_POP(arr) \
	((arr).data[--(arr).size])
// USAGE:
// int x = DYNAMIC_ARRAY_POP(xs);
// x is now equal to the last element of xs, and the last element is gone from xs

// NOTE: if you're wondering why we're using predecrement (--arr->size instead of arr->size--)  
// ^ its because of 0-indexing.
// NOTE: popping just removes an element and shrinks size, but it doesnt shrink capacity
// if you have a really big array and pop everything out, the capacity stays the same
// that memory is still allocated for the array! << keep this in mind
// we want pop and push to be giga fast, so if we did auto shrinking of capacity it would kill performance
// also, we can't add auto shrinking because this needs to evaluate to the value of the popped item
// if it didnt, it wouldnt be very useful 

// USAGE:
// DYNAMIC_ARRAY(int) xs;
// Declares a dynamic array, don't use it yet! its not initialized! 
// Data is a nullpointer! 
// NOTE: now when you initilize a dynamic array you specify a typename also;



#define DYNAMIC_ARRAY_RECALCULATE_CAPACITY(arr)									\
do {																			\
	(arr).capacity = (arr).size;												\
	(arr).data = realloc( (arr).data, (arr).capacity * sizeof(*(arr).data));	\
} while(0)
// USAGE:
// DYNAMIC_ARRAY_RECALCULATE_CAPACITY(xs);
// optimizes capacity to be as small as possible
// reallocates the array so we dont have a bunch of extra bytes allocated to it and unused

// NOTE: it may be tempting to spam this, but keep in mind allocation overhead vs memory usage tradeoff
// so use it when its necessary, like after you pop everything or clear everything in a large array 


#define DYNAMIC_ARRAY_CLEAR(arr)	\
do {								\
	(arr).size = 0;					\
} while(0)
// USAGE:
// DYNAMIC_ARRAY_CLEAR(xs);
// makes the array empty

// NOTE: does not actually remove any items from the array, and doesnt touch capacity, only changes size!
// for example take an array of size 10, if we clear and then try to access item 10, its still there
// the size is 0 now, so item 10 is garbage memory now (not really but in practice its like that)
// pushing 10 new items overwrites the old items as if we were pushing into an empty array
// 
// ^ that's technical, in practice, it deletes everything in the array giga fast


#define DYNAMIC_ARRAY_RESET(arr)		\
do {									\
	(arr).size = 0;						\
	(arr).capacity = 0;					\
	free((arr).data);					\
	(arr).data = NULL;					\
} while(0)
// USAGE:
// DYNAMIC_ARRAY_RESET(xs);
// SUPER makes the array empty, clears all items and frees the memory
//
// NOTE: this is the giga strong version of clear, it's slower, so dont spam
// (very very slightly slower, probably doesnt matter unless its being run 1000x per second)
// NOTE: this array can be used still, the free doesnt destroy it, it just removes the data
// you can push items on to this array as if it were brand new, the array isn't gone
// but the memory is gone, so you can reset an array and then walk away from it and it wont cause a memory leak


#define DYNAMIC_ARRAY_FREE(arr)			\
do {									\
	free((arr).data);					\
} while(0)
// USAGE:
// DYNAMIC_ARRAY_FREE(xs);
// kills the array 

// NOTE: this is like a faster version of reset, but the pointer is gone now, you cant use it again! 


#define DYNAMIC_ARRAY_RESERVE(arr, newcapacity)												\
do {																						\
	if ((newcapacity) > (arr).capacity) {													\
		(arr).capacity = (newcapacity)														\
		(arr).data = realloc( (arr).data, (arr).capacity * sizeof(*(arr).data));			\
	}																						\
} while(0)
// USAGE:
// DYNAMIC_ARRAY_RESERVE(xs);
// Guarantees that the array has at least a certain capacity

// NOTE: if the newcapacity is smaller than the current capacity then nothing happens

#define DYNAMIC_ARRAY_INSERT(arr, value, index)                                         \
do {                                                                                    \
    if ((arr).size == (arr).capacity){                                                  \
        (arr).capacity = (arr).capacity == 0 ? 8 : (arr).capacity * 2;              \
        (arr).data = realloc((arr).data, (arr).capacity * sizeof(*(arr).data));            \
    }                                                                                   \
    for (size_t _i = (arr).size; _i > (index); _i--){                                   \
        (arr).data[_i] = (arr).data[_i - 1];                                            \
    }                                                                                   \
    (arr).data[(index)] = (value);                                                      \
    (arr).size++;                                                                       \
} while(0)

// puts a value at a specific index, growing if index is greater than size or capacity 
// Using _i instead of i to reduce the chance of it interfering with an existing i
// I have decied to use a loop instead of memmove because memove requires teh string.h library
// yes memmove will be faster but to use this macro you'd then have to remmber to include string.h 
// uses the same  resize 2* or 8 when empty as push 

#define DYNAMIC_ARRAY_ERASE(arr, index)													\
do {																					\
	for(size_t _i == index; _i < ((arr).size-1); ++_i){									\
		(arr).data[_i] = (arr).data[_i + 1];											\
	}																					\
	--(arr).size; 																		\
}while(0)																				
// removes an item from an array
// shifts all the items to the right of it to fill the gap
// preserves ordering 


#define DYNAMIC_ARRAY_ERASE_RANGE(arr, rangestart, rangeend)							\
do {																					\
	size_t rangedif = rangestart-rangeend;												\
	for()																				\
		for(size_t _i == index; _i < ((arr).size-rangedif); ++_i){						\
		(arr).data[_i] = (arr).data[_i + rangedif];										\
	}																					\
	(arr).size -= rangestart - rangeend; 												\
}while(0)																				\
// like remove, but removes more than 1 element
// requires shifting, so slow

#define DYNAMIC_ARRAY_APPEND(arr1, arr2)  												\
do {																					\
	size_t _newsize = (arr1).size + (arr2).size; 										\
	if ((arr1).capacity <  _newsize){													\
		(arr1).capacity = (arr1).size <= (arr2).size ? ((arr2).size == 0 ? 8: 2*(arr2).size) : 2*(arr1).size; \
		(arr1).data = realloc((arr1).data, (arr1).capacity* sizeof(*(arr).data));		\
	}																					\																		\
	for(size_t _i == (arr1).size; i < _newsize +; ++_i){								\
		(arr1).data[i] = (arr2).data(_newsize - (arr1).size)							\
	}																					\
		(arr1).size = _newsize;															\
} while(0)																				\

// attaches array 2 onto the end of array 1, like we pushed every item of array 2 onto array 1, in order 													\
// NOTE: maybe not the best implementation because it requires a bunch of resize checks for every inserted item 
// it is the simplest implementation tho because I wasn't sure how to handle resizing I could just double 
// I could just double the size untill its enough to fit or something imma leave it in the TODO section for now
// it will work but it could be made better

*/ 
#endif
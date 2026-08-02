
#ifndef DARRAY_H
#define DARRAY_H
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//README
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// INTRO:  	 | This h file contains two macros, they generates a dynamic array and a set of functions to manage
// 		  	 | the dynamic array of that type.
//			 |
// USE:	 	 | DYNAMIC_ARRAY(type, typename) type is the type of the elements of the array and typename is the name of the array
//			 | eg DYNAMIC_ARRAY(char, string) produces a dynamic array of chars called string
//			 |	
// IMPORTANT:| DYNAMIC_ARRAY_MULTILEVEL(type, typename, free_function) is for custom data types that contain 
//			 | heap allocated data as to ensure no memory leaks. If using DYNAMIC_ARRAY_MULTILEVEL on a custom data you must 
//           | provide a function into free_function which takes (A*) where A is the name of the custom data type which handles 
//			 | freeing any data contained within for using a darray of adarray free_function would be darray_free where
//			 | darray is the name of the array
//			 |  
// NOTE: 	 | There is no out of bounds indexing check on everything thats not erase due to returning false being reserved 
//			 | for allocation error meaning it is up to the user to ensure their indexes are valid
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 


//List of functions
//_init(*arr, size, *c_arr) Initilizes the array if passed a NULL ptr in the c_arr argument it will initilize to size 0, capacity 0
//_push(*arr, value) Pushes a item onto the end of the array 
//_pop(*arr) pops the last value off of the array
//_recalculate_capacity(*arr) resizes teh capacity to match the size
//_clear(*arr) resizes the array back to 0
//_reset(*arr) resizes and frees data
//_reserve(*arr, newcapacity) reserves more uninitilized memory
//_insert(*arr, value, index) inserts value at index
//_erase(*arr, index) removes the item at index
//_erase_range(*arr, rangestart, rangeend) erases range beteen rangestart and rangeend
//_array_append(*arr1, *arr2) appends arr2 and the end of arr1
//_c_array_appent(*arr, *c_arr) appends c array on the end of arr1
// MULTILEVEL
//_deep_erase(*arr, index) same as regular erase execpts calls type_free() for each element to ensure no memory leakage
//_deep_erase_range(*arr, rangestart, rangeend) same as deep_erase but over a range




/*
	This section of macros are the actual implementation of the logic in the functions of the main macros. 

	This approach was decided on due to having multiple macros that implement the code and to break up the structure 
	to make commenting easyer as with a large macro you can't comment after each function due to the \.

	No do {} while(0)s where used because these macros are only to be used within the header so it is known exactly how
	they will be used and adding them may make already hard to read macros even harder. 
	
	Due to containing text such as *arr, value, *c_arr or buffer a function signature of the function it would be  
	used for is provided above the macro to show the types of these values and what the function returns
*/


#define ___DECLARE_ARRAY(type, typename) typedef struct typename{ type *data; size_t size; size_t capacity;} typename; 
// declares the struct containing a data array pointer which will point to the raw data, a size value containing how many items
// are contained in the array, and a capacity value containing how many items can fit in allocated space 


//typename typename_empty() (little confusing it returns typename and the function would be called typename_empty)
#define _EMPTY(type, typename)        \
		typename arr;                 \
		arr.data = NULL;              \
		arr.size = 0;                 \
		arr.capacity = 0;             \
		return arr;
// returns an empty array             

//bool typename_init(typename* arr, size_t buffer, type *c_arr)
#define ___INIT(type, typename)                                             \
		if (c_arr == NULL){                                                 \
			arr->data = NULL;                                               \
			arr->capacity = 0;                                              \
			arr->size = 0;                                                  \
		}else{                                                              \
			arr->data = (type*)malloc(arr->capacity * sizeof(type));        \
			if(arr->data){                                                  \
				memcpy(arr->data, c_arr, buffer * sizeof(type));            \
				arr->size = buffer;                                         \
				arr->capacity = buffer;                                     \
			}else{                                                          \
				arr->capacity = 0;                                          \
				arr->size = 0;                                              \
				return false;                                               \
			}                                                               \
		}
//Initilizes the array with a c_array

//bool typename_homogenous_init(typename *arr, size_t size, type item)
#define ___HOMOGENOUS_INIT(type, typename)                     \
		arr->data = (type*)malloc(size * sizeof(type));        \
		if(arr->data){                                         \
			arr->size = size;                                  \
			arr->capacity = size;                              \
			for(size_t i = 0; i < size; ++i){                  \
				arr->data[i] = item;                           \
			}                                                  \
			return true;                                       \
		}else{                                                 \
			arr->capacity = 0;                                 \
			arr->size = 0;                                     \
			return false;                                      \
		}
//Initilizes the array with one repeating item

// bool typename_reinit(typename *arr, size_t buffer, type* c_arr)
#define ___REINIT(type, typename)                                              \
		if(arr->data == NULL){                                                 \
			___INIT(type, typename)                                            \
		}else{                                                                 \
			type* tmp = (type*)realloc(arr->data, buffer*sizeof(type));        \
			if(tmp){                                                           \
				arr->data = tmp;                                               \
				memcpy(arr->data, c_arr, buffer*sizeof(type));                 \
				arr->capacity = buffer;                                        \
				arr->size = buffer;                                            \
				return true;                                                   \
			}else{                                                             \
				return false;                                                  \
			}                                                                  \
		}
//reinitilizes teh array with a c_arr

// bool typename_homogenous_reinit(typename *arr, size_t size, type item)
#define ___HOMOGENOUS_REINIT(type, typename)                                 \
		if(arr->data == NULL){                                               \
			___HOMOGENOUS_INIT(type,typename)                                \
		}else{                                                               \
			type* tmp = (type*)realloc(arr->data, size*sizeof(type));        \
			if(tmp){                                                         \
				arr->data = tmp;                                             \
				arr->size = size;                                            \
				arr->capacity = size;                                        \
				for(size_t i = 0; i < size; ++i){                            \
					arr->data[i] = item;                                     \
				}                                                            \
				return true;                                                 \
			}else{                                                           \
				return false;                                                \
			}                                                                \
		}

// bool typename_push(typename *arr, type item)
#define ___PUSH(type, typename)                                             \
		if(arr->size >= arr->capacity){                                     \
			size_t capacity = arr->capacity ? arr->capacity * 2 : 8;        \
			type* tmp = realloc(arr->data, capacity * sizeof(type));        \
			if(tmp == NULL){                                                \
				return false;                                               \
			}                                                               \
			arr->data = tmp;                                                \
			arr->capacity = capacity;                                       \
		}                                                                   \
		arr->data[arr->size++] = item;                                      \
		return true;

// type typename_pop(typename *arr, type item)
#define ___POP(type, typename)												\
		return arr->data[--arr->size];
// pops the last item off the array 

// bool typename_recalculate_capacity(typename* arr)
#define ___RECALCULATE_CAPACITY(type, typename)                                    \
		type* tmp = (type*)realloc(arr->data, arr->capacity * sizeof(type));       \
		if(tmp){                                                                   \
			arr->data = tmp;                                                       \
			arr->capacity = arr->size;                                             \
			return true;                                                           \
		}                                                                          \
		return false;
//returns array capacity to its current size


// void typename_clear(*typename arr)
#define ___CLEAR(type, typename)\
		arr->size = 0;
// clears the array but retains its capacity

// void typename_reset(typename *type)
#define ___RESET(type, typename)        \
		arr->size = 0;                  \
		arr->capacity = 0;              \
		free(arr->data);                \
		arr->data = NULL;
//fully resets the array frees data and sets size and capacity to 0

// void typename_free(typename* arr)
#define ___FREE(type, typename)		\
		free(arr->data);
//only frees data to be used when the array is being discarded

//bool reserve(typename )
#define ___RESERVE(type, typename)                                           \
	if(arr->capacity == 0 && newcapacity > 0){                               \
		arr->capacity = newcapacity;                                         \
		arr->data = (type*)malloc(arr->capacity * sizeof(type));             \
		if(arr->data == NULL){                                               \
			return false;                                                    \
		}                                                                    \
	}                                                                        \
                                                                             \
	else if(newcapacity > arr->capacity){                                    \
		arr->capacity = newcapacity;                                         \
		type* tmp = realloc(arr->data, arr->capacity * sizeof(type));        \
		if(tmp == NULL){                                                     \
			return false;                                                    \
		}                                                                    \
	}                                                                        \
	return true;
	
// bool typename_insert(typename* arr, size_t index, type item)
#define ___INSERT(type, typename)                                                             \
		if(arr->size == arr->capacity){                                                       \
			size_t capacity = (arr->capacity ? arr->capacity*2 : 8);                          \
			type* tmp = (type*)realloc(arr->data, capacity * sizeof(type));                   \
			if(tmp == NULL){                                                                  \
				return false;                                                                 \
			}                                                                                 \
			arr->data = tmp;                                                                  \
			arr->capacity = capacity;                                                         \
		}                                                                                     \
		memmove(arr->data+index+1, arr->data+index, (arr->size-index) * sizeof(type));        \
		arr->data[index] = item;                                                              \
		arr->size++;                                                                          \
		return true;
// inserts an intem into the array at the given index

// bool typename_insert_c_array(typename* arr, size_t index, size_t buffer, type* c_arr)                              
#define ___INSERT_C_ARRAY(type, typename)                                                                             \
		if(arr->size + buffer > arr->capacity){                                                                       \
			size_t capacity = arr->capacity*2 > (arr->size + buffer) ? arr->capacity*2  :  arr->size + buffer;        \
			type* tmp = (type*)realloc(arr->data, capacity * sizeof(type));                                           \
			if(tmp == NULL){                                                                                          \
				return false;                                                                                         \
			}                                                                                                         \
			arr->data = tmp;                                                                                          \
			arr->capacity = capacity;                                                                                 \
		}                                                                                                             \
		memmove(arr->data+index+buffer, arr->data+index, (arr->size-index) * sizeof(type));                           \
		memcpy(arr->data+index+buffer, c_arr, buffer * sizeof(type));                                                 \
		arr->size+=buffer;                                                                                            \
		return true;
// inserts a standard c array of size buffer starting at index

// bool typename_insert_array(typename* arr, size_t index, typename* arr2)
#define ___INSERT_ARRAY(type, typename)          \
		type* c_arr = arr2->data;                \
		size_t buffer = arr2->size;              \
		___INSERT_C_ARRAY(type, typename)
// inserts arr2 into arr1 starting at index

// bool typename_c_array_appened(typename* arr, size_t buffer, type* c_arr)
#define ___C_ARRAY_APPEND(type, typename)                                                                             \
		if(arr->size + buffer > arr->capacity){                                                                       \
			size_t capacity = arr->capacity*2 > (arr->size + buffer) ? arr->capacity*2  :  arr->size + buffer;        \
			type* tmp = (type*)realloc(arr->data, capacity * sizeof(type));                                           \
			if(tmp == NULL){                                                                                          \
				return false;                                                                                         \
			}                                                                                                         \
			arr->data = tmp;                                                                                          \
			arr->capacity = capacity;                                                                                 \
		}                                                                                                             \
		memcpy(arr->data, c_arr, buffer * sizeof(type));                                                              \
		arr->size+=buffer;

#define ___ARRAY_APPEND(type, typename)          \
		type* c_arr = arr2->data;                \
		size_t buffer = arr2->size;              \
		___C_ARRAY_APPEND(type, typename)

// bool typename_erase(typename* arr, size_t index)
#define ___ERASE(type, typename)                                                                \
        if(index >= arr->size ){                                                                \
			return false;                                                                       \
		}                                                                                       \
		memmove(arr->data+index, arr->data+index + 1, (arr->size-index) * sizeof(type));        \
		arr->size--;                                                                            \
		return true;
// erases the item at index if index is greater than the size of the array returns false 


#define ___ERASE_RANGE(type, typename)                                                                                    \
		if (rangestart > rangeend || rangeend > arr->size){                                                               \
			return false;                                                                                                 \
		}                                                                                                                 \
		size_t reduction = rangeend - rangestart;                                                                         \
		memmove(arr->data + rangestart, arr->data + rangestart + rangeend, (arr->size - rangeend) * sizeof(type));        \
		arr->size -= reduction;                                                                                           \
		return true;
//erases a range of elements starting at range start and ending at range end returns false if rangeend < rangestart or rangeend > arr->size

#define ___MLREINIT(type, typename, free_function)        \
		for(size_t i = 0; i < arr->size; ++i){            \
			 free_function(&arr->data[i]);                \
		}                                                 \
		___REINIT(type, typename)
#define ___MLHOMOGENOUS_REINIT(type, typename, free_function)        \
		for(size_t i = 0; i < arr->size; ++i){                       \
			 free_function(&arr->data[i]);                           \
		}                                                            \
		___HOMOGENOUS_REINIT(type, typename)

#define ___MLFREE(type, typename, free_function)        \
		for(size_t i = 0; i < arr->size; ++i){          \
			 free_function(&arr->data[i]);              \
		}                                               \
		free(arr->data);

#define ___MLCLEAR(type, typename, free_function)        \
		for(size_t i = 0; i < arr->size; ++i){           \
			 free_function(&arr->data[i]);               \
		}                                                \
		arr->size = 0;
#define ___MLRESET(type, typename, free_function)        \
		for(size_t i = 0; i < arr->size; ++i){           \
			 free_function(&arr->data[i]);               \
		}                                                \
		___RESET(type, typename)

#define ___DEEP_ERASE(type, typename, free_function)                                            \
		if(index >= arr->size ){                                                                \
			return false;                                                                       \
		}                                                                                       \
		free_function(arr->data+index)                                                          \
		memmove(arr->data+index, arr->data+index + 1, (arr->size-index) * sizeof(type));        \
		arr->size--;                                                                            \
		return true;

#define ___DEEP_ERASE_RANGE(type, typename, free_function)                                                                \
		if (rangestart > rangeend || rangeend > arr->size){                                                               \
			return false;                                                                                                 \
		}                                                                                                                 \
		size_t reduction = rangeend - rangestart;                                                                         \
		for(size_t i = rangestart; i < rangeend; i++){                                                                    \
			free_function(arr->data+i);                                                                                   \
		}                                                                                                                 \
		memmove(arr->data + rangestart, arr->data + rangestart + rangeend, (arr->size - rangeend) * sizeof(type));        \
		arr->size -= reduction;                                                                                           \
		return true;


#define DYNAMIC_ARRAY(type, typename)                                                                                                               \
	___DECLARE_ARRAY(type, typename)                                                                                                                   \
	static inline typename typename##_empty(){_EMPTY(type, typename)}                                                                               \
	static inline bool typename##_init(typename *arr, size_t buffer, type* c_arr){___INIT(type, typename)}                                          \
	static inline bool typename##_homogenous_init(typename *arr, size_t size,  type item){___HOMOGENOUS_INIT(type, typename)}                       \
	static inline bool typename##_reinit(typename *arr, size_t buffer, type* c_arr){___REINIT(type, typename)}                                      \
	static inline bool typename##_homogenous_reinit(typename *arr, size_t size,  type item){___HOMOGENOUS_REINIT(type, typename)}                   \
	static inline bool typename##_push(typename *arr, type item){___PUSH(type, typename)}                                                           \
	static inline type typename##_pop(typename *arr){___POP(type,typename)}                                                                         \
	static inline bool typename##_recalculate_capacity(typename *arr){___RECALCULATE_CAPACITY(type,typename)}                                       \
	static inline void typename##_clear(typename *arr){___CLEAR(type,typename)}                                                                     \
	static inline void typename##_reset(typename *arr){___RESET(type, typename)}                                                                    \
	static inline void typename##_free(typename *arr){___FREE(type, typename)}                                                                      \
	static inline bool typename##_reserve(typename *arr, size_t newcapacity){___RESERVE(type, typename)}                                            \
	static inline bool typename##_insert(typename *arr, size_t index, type item){___INSERT(type,typename)}                                          \
	static inline bool typename##_insert_c_array(typename* arr, size_t index, size_t buffer, type* c_arr){___INSERT_C_ARRAY(type, typename)}        \
	static inline bool typename##_insert_array(typename* arr, size_t index, typename* arr2){___INSERT_ARRAY(type, typename)}                        \
	static inline bool typename##_c_array_append(typename* arr, size_t buffer, type* c_arr){___C_ARRAY_APPEND(type, typename)}                      \
	static inline bool typename##_array_append(typename* arr, typename* arr2){___ARRAY_APPEND(type, typename)}                                      \
	static inline bool typename##_erase(typename *arr, size_t index){___ERASE(type,typename)}                                                       \
	static inline bool typename##_erase_range(typename *arr, size_t rangestart, size_t rangeend){___ERASE_RANGE(type, typename)}
	  
																							

//==================================================================================================================================
// MULTI LEVEL FOR WHEN THE TYPE CONTAINED IN THE ARRAY CONTAINS A HEAP POINTER AND REQUIRES EXTRA FREEING TO ENSURE NO MEM LEAKS 
//==================================================================================================================================

#define DYNAMIC_ARRAY_MULTILEVEL(type, typename, free_function)				\
	typedef struct { type *data; size_t size; size_t capacity;} typename;	\
	static inline void typename##_init(typename *arr, size_t size, type* c_arr){	\
		if (c_arr == NULL){													\
			arr->data = NULL;												\
			arr->size = 0;													\
			arr->capacity = 0;												\
		}else{																\
			arr->size = size;												\
			arr->capacity = size;											\
			arr->data = (type*)malloc(arr->capacity * sizeof(type));		\
			memcpy(arr->data, c_arr, size * sizeof(type));					\
		}																	\
	}																		\
																			\
	static inline void typename##_push(typename *arr, type value){			\
		if(arr->size >= arr->capacity){										\
			arr->capacity = arr->capacity ? arr->capacity * 2 : 8;			\
			arr->data = realloc(arr->data, arr->capacity);					\
		}																	\
		arr->data[arr->size++] = value;										\
	}																		\
	static inline type typename##_pop(typename *arr){						\
		type tmp = arr->data[--arr->size];									\
		return tmp; 														\
	}																		\
	static inline void typename##_recalculate_capacity(typename *arr){		\
		arr->capacity = arr->size;											\
		arr->data = realloc(arr->data, arr->capacity * sizeof(type));		\
	}																		\
	static inline void typename##_clear(typename *arr){						\
		for(size_t i = 0; i < arr->size; ++i){								\
			 free_function(&arr->data[i]); 								\
		}																	\
		arr->size = 0;														\
	}																		\
	static inline void typename##_reset(typename *arr){						\
		arr->size = 0;														\
		arr->capacity = 0;													\
		for(size_t i = 0; i < arr->size; ++i){								\
			 free_function(&arr->data[i]);									\
		}																	\
		free(arr->data);													\
		arr->data = NULL;													\
	}																		\
	static inline void typename##_free(typename *arr){						\
		for(size_t i = 0; i < arr->size; ++i){								\
			 free_function(&arr->data[i]);									\
		}																	\
		free(arr->data);													\
	}																		\
	static inline void typename##_reserve(typename *arr, size_t newcapacity){		\
		if(newcapacity > arr->capacity){									\
			arr->capacity = newcapacity;									\
			arr->data = realloc(arr->data, arr->capacity * sizeof(type));	\
		}																	\
	}																		\
																			\
	static inline void typename##_insert(typename *arr, type value, size_t index){	\
		if(arr->size == arr->capacity){										\
			arr->capacity = arr->capacity ? arr->capacity*2 : 8;			\
			arr->data = realloc(arr->data, arr->capacity * sizeof(type)); 	\
		}																	\
		memmove(arr->data+index+1, arr->data+index, arr->size-index);		\
		arr->data[index] = value;											\
		arr->size++;														\
	}																		\
	static inline void typename##_erase(typename *arr, size_t index){				\
		memmove(arr->data+index + 1, arr->data+index + 1, (arr->size-index) * sizeof(type));	\
		arr->size--;														\
	}																		\
	static inline void typename##_erase_range(typename *arr, size_t rangestart, size_t rangeend){	\
		if (rangestart > rangeend){												 	\
			return;																	\
		}																			\
		for(size_t i = rangestart; i < rangeend && rangeend + i < arr->size; i++){	\
			arr->data[i + rangestart] = arr->data[i + rangeend];					\
		}																			\
		memmove(arr->data+rangestart, arr->data+rangeend, (arr->size - rangeend)* sizeof(type));	\
		arr->size -= rangeend - rangestart; 										\
	}																				\
	static inline void typename##_deep_erase(typename *arr, size_t index){						\
		 free_function(&arr->data[index]);										\
		memmove(arr->data+index + 1, arr->data+index + 1, (arr->size - index) * sizeof(type));\
		--arr->size;														\
	}																		\
	static inline void typename##_deep_erase_range(typename *arr, size_t rangestart, size_t rangeend){	\
		if (rangestart > rangeend){												 	\
			return;																	\
		}																			\
		size_t rangedif = rangeend-rangestart;										\
		for(size_t i = rangestart; i < rangeend; ++ i){								\
			 free_function(&arr->data[i]);												\
			arr->data[i] = arr->data[i + rangedif];									\
		}																			\
		memmove(arr->data+rangestart, arr->data+rangeend, (arr->size - rangeend)*sizeof(type));	\
		arr->size -= rangeend - rangestart; 										\
	}																				\
	static inline void typename##_array_append(typename *arr1, typename *arr2){					\
		size_t newsize = arr1->size + arr2->size;									\
		if(arr1->capacity < newsize){												\
			arr1->capacity = arr1->size <= arr2->size ? (arr2->size ? 2*arr2->size : 8) : 2*arr1->size; \
			arr1->data = realloc(arr1->data, arr1->capacity * sizeof(*(arr1->data)));\
		}																			\
		memcpy(arr1->data + arr1->size, arr2->data, (arr2->size)*sizeof(type));					\
		arr1->size = newsize;														\
	}																				\
	static inline void typename##_c_array_append(typename *arr1, const type *arr2, const size_t buffer){ \
		size_t newsize = arr1->size + buffer;												\
		if(arr1->capacity < newsize){														\
			arr1->capacity = arr1->size <= buffer ? (buffer ? 2*buffer : 8) : 2*arr1->size; \
			arr1->data = realloc(arr1->data, arr1->capacity * sizeof(*(arr1->data)));		\
		}																					\
		memcpy(arr1->data + arr1->size, arr2, buffer * sizeof(type));										\
		arr1->size = newsize;																\
	}																						\


// #undef _EMPTY
// #undef _INIT
// #undef _HOMOGENOUS_INIT
// #undef _REINIT
// #undef _PUSH
// #undef _POP
// #undef _RECALCULATE_CAPACITY
// #undef _CLEAR
// #undef _RESET
// #undef _INSERT
// #undef _INSERT_C_ARRAY
// #undef _INSERT_ARRAY
// #undef _ERASE
// #undef _ERASE_RANGE
// #undef _C_ARRAY_APPEND
// #undef _ARRAY_APPEND
// #undef _RESET
// #undef _FREE
// #undef _MLFREE
// #undef _MLCLEAR
// #undef _MLRESET
// #undef _DEEP_ERASE
// #undef _DEEP_ERASE_RANGE

#endif
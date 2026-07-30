
#ifndef DARRAY_H
#define DARRAY_H
#include <string.h>
#include <stdlib.h>

//README
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// INTRO:  	 | This h file contains two macros, they generates a dynamic array and a set of functions to manage
// 		  	 | the dynamic array of that type.
//			 |
// USE:	 	 | DYNAMIC_ARRAY(type, typename) type is the type of the elements of the array and typename is the name of the array
//			 | eg DYNAMIC_ARRAY(char, string) produces a dynamic array of chars called string
//			 |	
// IMPORTANT:| DYNAMIC_ARRAY_MULTILEVEL(type, typename) is for custom data types that contain 
//			 | heap allocated data as to ensure no memory leaks. If using DYNAMIC_ARRAY_MULTILEVEL on a custom data type
//			 | a function called A_free(A* a) where A is the name of the custom data type which handles freeing any data 
//			 | contained within must exist 
//			 |
// NOTE: 	 | its not perfectly safe since there are no checks on realloc and malloc so it can fail silently. 
//			 | and there are no checks on out of bounds indexing ect ect
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


#define DYNAMIC_ARRAY(type, typename)									 	\
	typedef struct typename{ type *data; size_t size; size_t capacity;} typename;	\
																			\
	static inline void typename##_init(typename *arr, size_t bufferSize, type* c_arr){	\
		if (c_arr == NULL){													\
			arr->data = NULL;												\
			arr->size = 0;													\
			arr->capacity = 0;												\
		}else{																\
			arr->size = bufferSize;												\
			arr->capacity = bufferSize;											\
			arr->data = (type*)malloc(arr->capacity * sizeof(type));		\
			memcpy(arr->data, c_arr, bufferSize * sizeof(type));					\
		}																	\
	}																		\
	static inline void typename##_reinit(typename *arr, size_t bufferSize, type* c_arr){ \
		if (c_arr == NULL){													\
			arr->size = 0;													\
		}else if(arr->data == NULL){											\
			typename##_init(arr, bufferSize, c_arr);						\
		}else{																\
			if(arr->capacity < bufferSize){									\
				arr->size = bufferSize;										\
				arr->capacity = bufferSize;									\
				arr->data = realloc(arr->data, arr->capacity);				\
			}																\
			memcpy(arr->data, c_arr, bufferSize * sizeof(type));					\
		}																	\
																			\
	}																		\
	static inline void typename##_push(typename *arr, type value){			\
		if(arr->size >= arr->capacity){										\
			arr->capacity = arr->capacity ? arr->capacity * 2 : 8;			\
			arr->data = realloc(arr->data, arr->capacity);					\
		}																	\
		arr->data[arr->size++] = value;										\
	}																		\
	static inline type typename##_pop(typename *arr){								\
		return arr->data[--arr->size];										\
	}																		\
	static inline void typename##_recalculate_capacity(typename *arr){				\
		arr->capacity = arr->size;											\
		arr->data = realloc(arr->data, arr->capacity * sizeof(type));		\
	}																		\
	static inline void typename##_clear(typename *arr){							\
		arr->size = 0;														\
	}																		\
	static inline void typename##_reset(typename *arr){							\
		arr->size = 0;														\
		arr->capacity = 0;													\
		free(arr->data);													\
		arr->data = NULL;													\
	}																		\
	static inline void typename##_free(typename *arr){						\
		free(arr->data);													\
	}																		\
	static inline void typename##_reserve(typename *arr, size_t newcapacity){		\
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
	static inline void typename##_insert(typename *arr, type value, size_t index){	\
		if(arr->size == arr->capacity){										\
			arr->capacity = arr->capacity ? arr->capacity*2 : 8;			\
			arr->data = realloc(arr->data, arr->capacity * sizeof(type)); 	\
		}																	\
		memmove(arr->data+index+1, arr->data+index, (arr->size-index) * sizeof(type));		\
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
		memmove(arr->data+rangestart, arr->data+rangeend, arr->size - rangeend);	\
		arr->size -= rangeend - rangestart; 										\
	}																				\
	static inline void typename##_array_append(typename *arr1, typename *arr2){					\
		size_t newsize = arr1->size + arr2->size;									\
																					\
		if(arr1->capacity < newsize){												\
			arr1->capacity = arr1->size <= arr2->size ? (arr2->size ? 2*arr2->size : 8) : 2*arr1->size; \
			arr1->data = realloc(arr1->data, arr1->capacity * sizeof(*(arr1->data)));\
		}																			\
		memcpy(arr1->data+arr1->size, arr2->data, arr2->size);					 	\
		arr1->size = newsize;														\
	}																				\
	static inline void typename##_c_array_append(typename *arr1, const type *arr2, const size_t buffer){ \
		size_t newsize = arr1->size + buffer;											\
		if(arr1->capacity < newsize){														\
			arr1->capacity = arr1->size <= buffer ? (buffer ? 2*buffer : 8) : 2*arr1->size; \
			arr1->data = realloc(arr1->data, arr1->capacity * sizeof(*(arr1->data)));		\
		}																					\
		memcpy(arr1->data + arr1->size, arr2, buffer * sizeof(type));										\
		arr1->size = newsize;																\
	}																						\
																							

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
			free_function##(&arr->data[i]); 									\
		}																	\
		arr->size = 0;														\
	}																		\
	static inline void typename##_reset(typename *arr){						\
		arr->size = 0;														\
		arr->capacity = 0;													\
		for(size_t i = 0; i < arr->size; ++i){								\
			free_function##(&arr->data[i]);										\
		}																	\
		free(arr->data);													\
		arr->data = NULL;													\
	}																		\
	static inline void typename##_free(typename *arr){								\
		for(size_t i = 0; i < arr->size; ++i){								\
			free_function##(&arr->data[i]);										\
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
	void typename##_deep_erase(typename *arr, size_t index){						\
		free_function##(&arr->data[index]);										\
		memmove(arr->data+index + 1, arr->data+index + 1, (arr->size - index) * sizeof(type));\
		--arr->size;														\
	}																		\
	void typename##_deep_erase_range(typename *arr, size_t rangestart, size_t rangeend){	\
		if (rangestart > rangeend){												 	\
			return;																	\
		}																			\
		size_t rangedif = rangeend-rangestart;										\
		for(size_t i = rangestart; i < rangeend; ++ i){								\
			free_function##(&arr->data[i]);												\
			arr->data[i] = arr->data[i + rangedif];									\
		}																			\
		memmove(arr->data+rangestart, arr->data+rangeend, (arr->size - rangeend)*sizeof(type));	\
		arr->size -= rangeend - rangestart; 										\
	}																				\
	void typename##_array_append(typename *arr1, typename *arr2){					\
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


#endif
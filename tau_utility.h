#ifndef _Tau_UTILITY_H_
#define _Tau_UTILITY_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

typedef enum
{
	Tau_FALSE,
	Tau_TRUE
} Tau_Bool;

#define Tau_ASSERT(expr) assert(expr)

#ifndef Tau_32BITNUMBERS
typedef float Tau_Float;
typedef int32_t Tau_Int;

#define Tau_StringToFloat strtof
#else
typedef double Tau_Float;
typedef int64_t Tau_Int;

#define Tau_StringToFloat strtod
#endif


/* Memory allocation functions */

#define Tau_USE_STD_ALLOC
#ifdef Tau_USE_STD_ALLOC

#define Tau_MALLOC(size)			malloc(size)
#define Tau_CALLOC(count, size)		calloc(count, size)
#define Tau_REALLOC(block, size)	realloc(block, size)
#define	Tau_ALLOC_TYPE(type)		(type*)calloc(1, sizeof(type))
#define Tau_FREE(block)				free(block)

#else

void* Tau_Malloc(const size_t size);
void* Tau_Calloc(const int count, const size_t size);
void* Tau_Realloc(void* block, const size_t size);
void Tau_Free(void* block);
#define Tau_MALLOC(size)			Tau_Malloc(size)
#define Tau_CALLOC(count, size)		Tau_Calloc(count, size)
#define Tau_REALLOC(block, size)	Tau_Realloc(block, size)
#define	Tau_ALLOC_TYPE(type)		Tau_Calloc(1, sizeof(type))
#define Tau_FREE(block)				Tau_Free(block)

#endif



/* Tau linked list implementation */

typedef struct Tau_ListNode
{
	struct Tau_ListNode* prev;
	struct Tau_ListNode* next;
} Tau_ListNode;

typedef struct
{
	Tau_ListNode*	begin;
	Tau_ListNode*	end;
	int				count;
} Tau_List;

#define Tau_LISTLINKSHEADER(type) struct type* prev; struct type* next

/**
 * @brief Pushes an element onto the beginning of a list.
 * @param list The list to push the element onto.
 * @param node Pointer to the element to push.
 */
void Tau_PushFrontList(
	Tau_List* const		list,
	Tau_ListNode* const	node
);

/**
 * @brief Pushes an element onto the end of a list.
 * @param list The list to push the element onto.
 * @param node Pointer to the element to push. Type must have list links header!
 */
void Tau_PushBackList(
	Tau_List* const		list,
	Tau_ListNode* const	node
);

/**
 * @brief Removes the element at the beginning of the list and returns a pointer to it.
 * @param list The list to get the element from.
 * @return A pointer to the element or NULL if the list was empty. Make sure to know
 * the type of the element returned to cast it from the void* returned.
 */
void* Tau_PopFrontList(
	Tau_List* const		list
);

/**
 * @brief Removes the element at the end of the list and returns a pointer to it.
 * @param list The list to get the element from.
 * @return A pointer to the element or NULL if the list was empty. Make sure to know
 * the type of the element returned to cast it from the void* returned.
 */
void* Tau_PopBackList(
	Tau_List* const		list
);

/**
 * @brief Removes a node from a list and links the surrounding elements together.
 * @param list The list to unlink from.
 * @param node The node to unlink from the list.
 */
void Tau_UnlinkFromList(
	Tau_List* const		list,
	Tau_ListNode* const	node
);

/**
 * @brief Removes all elements from a list and calling the destroy_func on every element.
 * This does not free the list itself, only empties it.
 * @param list Pointer to the list to empty.
 * @param destroy_func Pointer to a destructor function, or NULL to use Tau_FREE().
 */
void Tau_ClearList(
	Tau_List* const list,
	void(*destroy_func)(void*)
);



/* String functions */
char* Tau_CopyCutString(
	const char* srcstring,
	int start,
	int length
);



#endif
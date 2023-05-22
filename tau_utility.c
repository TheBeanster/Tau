#include "tau_utility.h"


/* Memory allocation functions */
#ifndef Tau_USE_STD_ALLOC

void* Tau_Malloc(const size_t size)
{
	void* block = malloc(size);
	if (!block)
	{
		printf("ERROR! : Couldn't allocate memory!\n");
		exit(-1);
	}
	return block;
}

void* Tau_Calloc(const int count, const size_t size)
{
	void* block = calloc(count, size);
	if (!block)
	{
		printf("ERROR! : Couldn't allocate memory!\n");
		exit(-1);
	}
	return block;
}

void* Tau_Realloc(void* block, const size_t size)
{
	block = realloc(block, size);
	if (!block)
	{
		printf("ERROR! : Couldn't rellocate memory!\n");
		exit(-1);
	}
	return block;
}

void Tau_Free(void* block)
{
	free(block);
}

#endif



/* Tau linked list implementation */

void Tau_PushFrontList(
	Tau_List* const		list,
	Tau_ListNode* const	node
)
{
	if (list->begin == NULL)
	{
		node->prev = NULL;
		node->next = NULL;
		list->begin = node;
		list->end = node;
	} else
	{
		node->prev = NULL;
		node->next = list->begin;
		list->begin->prev = node;
		list->begin = node;
	}
	list->count++;
}

void Tau_PushBackList(
	Tau_List* const		list,
	Tau_ListNode* const	node
)
{
	if (list->begin == NULL)
	{
		node->prev = NULL;
		node->next = NULL;
		list->begin = node;
		list->end = node;
	} else
	{
		node->prev = list->end;
		node->next = NULL;
		list->end->next = node;
		list->end = node;
	}
	list->count++;
}

void* Tau_PopFrontList(Tau_List* const list)
{
	Tau_ListNode* node = list->begin;
	Tau_UnlinkFromList(list, node);
	return node;
}

void* Tau_PopBackList(Tau_List* const list)
{
	Tau_ListNode* node = list->end;
	Tau_UnlinkFromList(list, node);
	return node;
}

void Tau_UnlinkFromList(
	Tau_List* const list,
	Tau_ListNode* const node
)
{
	if (node->prev)
		node->prev->next = node->next;
	else
		list->begin = node->next;
	if (node->next)
		node->next->prev = node->prev;
	else
		list->end = node->prev;
	node->prev = NULL;
	node->next = NULL;
	list->count--;
}

void Tau_ClearList(
	Tau_List* const list,
	void(*destroy_func)(void*)
)
{
	if (!list) return;
	if (list->count <= 0) return;
	if (list->begin == NULL) return;
	if (list->end == NULL) return;

	Tau_ListNode* iterator = list->begin;
	while (iterator)
	{
		Tau_ListNode* delblock = iterator;
		iterator = iterator->next;
		if (destroy_func)
			destroy_func(delblock);
		else
			Tau_FREE(delblock);
	}
	list->begin = NULL;
	list->end = NULL;
	list->count = 0;
}





char* Tau_CopyCutString(const char* srcstring, int start, int length)
{
	char* str = Tau_MALLOC(length + 1); /* Plus 1 to include null terminator */
	for (int i = 0; i < length; i++)
		str[i] = srcstring[i + start];
	str[length] = '\0';
	return str;
}

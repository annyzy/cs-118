 //NAME: Ziying Yu
 //EMAIL: annyu@g.ucla.edu
 //ID: 105182320

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <assert.h>

#include "SortedList.h"

//int opt_yield = 0;
void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
	if (list ==NULL || element ==NULL)
		return;

	SortedListElement_t *temp = list->next;
	while(temp != list)
	{
		if (strcmp(temp->key, element->key) < 0)
//			break;
			temp = temp->next;
		else
			break;
//			temp = temp->next;
	}
	
	if (opt_yield & INSERT_YIELD)
		sched_yield();

	element->next = temp;
	element->prev = temp->prev;
	temp->prev->next = element;
	temp->prev = element;
}

int SortedList_delete( SortedListElement_t *element)
{
	if(element== NULL)
		return 1;

	if(element->next->prev != element || element->prev->next != element)
		return 1;

	if (opt_yield & DELETE_YIELD)
		sched_yield();

	element->prev->next = element->next;
	element->next->prev = element->prev;

	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
	if(list == NULL || key == NULL)
		return NULL;

	SortedListElement_t *temp = list->next;

//	while (temp != NULL)
//	{
//		if (opt_yield & LOOKUP_YIELD)
//			sched_yield();
//		if ( strcmp(temp->key,key) == 0)
//			return temp;
//		temp = temp -> next;
//	}

	while (strcmp(temp->key, key) != 0)
	{
		if (temp == list)
		{
			return NULL;
		}
		if (opt_yield & LOOKUP_YIELD)
		{
			sched_yield();
		}
		temp = temp->next;
	}

	return temp;
}

int SortedList_length(SortedList_t *list)
{
	if(list == NULL)
		return -1;

	int length;
	length = 0;

	SortedListElement_t *temp = list->next;
	while (temp != list)
	{
		if (temp == NULL || temp->prev == NULL || temp->next == NULL || temp->prev->next != temp || temp->next->prev != temp)
		{
			return -1;
		}
		if (opt_yield & LOOKUP_YIELD)
		{
			sched_yield();
		}
		length++;
		temp = temp->next;
	}

	return length;

}



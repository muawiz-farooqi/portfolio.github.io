// The MIT License (MIT)
// 
// Copyright (c) 2021, 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES UTA OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

// maximum size of linked list set to 10000
#define MAX_LINKED_LIST_SIZE 10000

int findFreeNodeInternal();
int insertNodeInternal(int previous, int current);
int firstFit(int size);
int bestFit(int size);
int worstFit(int size);
int nextFit(int size);

// enumerator type to define if a node is a hole or process
enum TYPE
{
	H = 0,
	P
};

// struct to build linked list nodes
struct Node
{ 
  	size_t size; 
	enum TYPE type;
  	void * arena;
  	int next; 
  	int previous;
  	int in_use;
};

// global linked list structure
static struct Node LinkedList[MAX_LINKED_LIST_SIZE];
// global algorithm variable to store algorithm of choice
static enum ALGORITHM global_algorithm;
// global arena to allocate memory to store linked list
static void * arena;
// global current used in next fit to access next hole block
static int global_current_nextFit = 0;
// rootNode that can be updated for linked list
static int rootNode = 0;
// initialized variable to check if init called before allocating memory
int initialized = 0;

// this function initializes a block of memory to store linked list items
int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
	// cannot allocate negative size
	if (size < 0)
	{
		return -1;
	}

	// align size to 4-bytes
	size_t requested_size = ALIGN4(size);
	
	// initialize all nodes in list to default values
	for (int i = 0; i < MAX_LINKED_LIST_SIZE; i++)
	{
		LinkedList[i].size = 0;
		LinkedList[i].type = H;
		LinkedList[i].arena = NULL;
		LinkedList[i].previous = -1;
		LinkedList[i].next = -1;
		LinkedList[i].in_use = 0;
		initialized = 1;
	}

	// allocate aligned size amount of memory to the global arena variable
	arena = malloc(requested_size);
	// set the global algorithm to the algorthm passed in
	global_algorithm = algorithm;
		
	// update the rootNode with the appropriate size, arena, next, previous, in_use
	LinkedList[rootNode].size = requested_size;
	LinkedList[rootNode].type = H;
	LinkedList[rootNode].arena = arena;
	LinkedList[rootNode].previous = -1;
	LinkedList[rootNode].next = -1;
	LinkedList[rootNode].in_use = 1;

	return 0;
}

// this function deallocates memory from the linked list
void mavalloc_destroy( )
{
	// set all nodes to default values
	for (int i = 0; i < MAX_LINKED_LIST_SIZE; i++)
	{
		LinkedList[i].size = 0;
		LinkedList[i].type = H;
		LinkedList[i].arena = 0;
		LinkedList[i].previous = -1;
		LinkedList[i].next = -1;
		LinkedList[i].in_use = 0;
	}

	// deallocate memory from the linked list
	free(arena);
	arena = NULL;

  	return;
}

// this function allocates memory using the chosen allocation algorithm
void * mavalloc_alloc( size_t size )
{
	// align size to 4 bytes
	size_t aligned_size = ALIGN4(size);
	int index = -1;

	// case statement for each allocation algorithm
	switch (global_algorithm)
	{
		// call the appropriate function and store the return index variable
		// that has the allocated memory
		case NEXT_FIT: 		// next fit
			index = nextFit(aligned_size);
			break;
		case BEST_FIT: 		// best fit
			index = bestFit(aligned_size);
			break;
		case WORST_FIT: 	// worst fit
			index = worstFit(aligned_size);
			break;
		case FIRST_FIT: 	// first fit
			index = firstFit(aligned_size);
			break;
		// error case: return NULL
		default:
			return NULL;
	}

	// if the index was allocated with a suitable index, return a pointer to the location in memory
	if (index != -1)
	{
		return LinkedList[index].arena;
	}
	// else return NULL
	else
	{
		return NULL;
	}

}

// thia function frees the location passed and combines free blocks
void mavalloc_free( void * ptr )
{
	// loop through all nodes
	for (int i = 0; i < MAX_LINKED_LIST_SIZE; i++)
	{
		// if node found
		if (LinkedList[i].arena == ptr)
		{
			// set block to hole type
			LinkedList[i].type = H;

			// define next and previous index variables
			int next_index = LinkedList[i].next;
			int previous_index = LinkedList[i].previous;

			// if previous or next index is a Hole and in use then combine blocks
			// set first block to size of both and change second block to default values
			if (LinkedList[next_index].in_use && LinkedList[next_index].type == H)
			{
				LinkedList[i].size += LinkedList[next_index].size;
				LinkedList[next_index].in_use = 0;
				LinkedList[next_index].size = 0;
				LinkedList[next_index].arena = NULL;
				LinkedList[i].next = LinkedList[next_index].next;				
			}
			if (LinkedList[previous_index].in_use && LinkedList[previous_index].type == H)
			{
				LinkedList[previous_index].size += LinkedList[i].size;
				LinkedList[i].in_use = 0;
				LinkedList[i].size = 0;
				LinkedList[i].arena = NULL;
				LinkedList[previous_index].next = LinkedList[i].next;				
			}		
		}
	}

  	return;
}

// this function checks and returns the size of the linked list
int mavalloc_size( )
{
  	int number_of_nodes = 0;

  	// loop through nodes
  	for (int i = 0; i < MAX_LINKED_LIST_SIZE; i++)
  	{
  		// if node in use
  		if (LinkedList[i].in_use)
  		{
  			// increase node counter
  			number_of_nodes++;
  		}
  	}

  	return number_of_nodes;
}

// internal function to find a node not in use
int findFreeNodeInternal()
{
	int i = 0;
	
	/**
	 *  Start searching the array beginning at the 0th element
	 *  and once we've found an element not in use we'll drop out 
	 *  and return the index that is free. 
	*/
	for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
	{
		if (LinkedList[i].in_use == 0)
		{
			return i;
		}
	}
	// if no index found, return -1
	return -1;
}

// internal function that places "current" node in front of "previous" node
int insertNodeInternal(int previous, int current)
{
	/**
	 *  Initialize our current links to an invalid index
	 *  -1 signifies the end of the list on either end
	 */
 	LinkedList[current].previous = -1;
	LinkedList[current].next = -1;

    /**
	 * Make sure we have a previous node.  If there
	 * was no previous node then the previous value
	 * would be -1
	 */
	if (previous >= 0)
	{
		int temp;
		/**
		 * Connect the current node with the previous node
		 * if it exists and store off previous->next.
		 * Then set previous->next to current
		 */
		LinkedList[current].previous = previous;

		temp = LinkedList[previous].next;
		LinkedList[previous].next = current;
		LinkedList[current].next = temp;
	
	}
	else if ( LinkedList[rootNode].previous == -1 && LinkedList[rootNode].next == -1)
	{
		/* Do nothing since this is the first node in the linked list. */
	}
	/**
	 * If we have a previous value -1 we're replacing the root node
	 * so after inserting it make sure to update the rootNode
	 */
	else
	{
		LinkedList[rootNode].previous = current;
		LinkedList[current].previous = -1;
		LinkedList[current].next = rootNode;
		rootNode = current;
	}

	return 0;
}

// this function uses passed in size to allocate memory using the first fit algorithm
int firstFit(int size)
{
    /** Index into the array where we will put this new node */
	int index    =  findFreeNodeInternal();   

    /** Set current to the rootNode so we start searching from the beginning */
	int current  =  rootNode;                 
	                                          
    /** Set previous to -1 since there nothing before the first node in the list */
	int previous = -1;                          
	                                          
    /** Our return value.  -1 on failure. 0 on success. */
	int ret      = -1;                        
	

	if (index >= 0)
	{
		/*	loop through linked list until an appropriate block that is in use,
			large enough, and of type hole*/
		while (current >= 0 &&
				!(LinkedList[current].type == H &&
				LinkedList[current].in_use &&
				LinkedList[current].size >= size))
		{
			// go to next node, update previous and current
			previous = current;
			current  = LinkedList[current].next;
		}		
		
        /** If we ran off the end of the list then insert on the tail of the list */
		if( current == -1 )
		{
			// store return val, if -1 then fail
			ret = insertNodeInternal(LinkedList[previous].previous, index);
		}
		else
		{
			// store return val, if -1 then fail
 			ret = insertNodeInternal(previous, index);
 		}

	    /**
	     * Now that our new node is linked in lets set the size and mark this array element
	     * as being used, change type to process and update location in memory and next pointer
	     */
 		LinkedList[index].type = P;
		LinkedList[index].in_use = 1;
		LinkedList[index].size = size;
		LinkedList[index].arena = LinkedList[current].arena;
		LinkedList[index].next = current;

		// split node if leftover space in block
		if (LinkedList[current].size > size)
		{
			LinkedList[current].type = H;
			LinkedList[current].in_use = 1;
			LinkedList[current].arena += size;
			LinkedList[current].previous = index;
			LinkedList[current].size -= size;
		}
		// empty node if no leftover space
		else if (LinkedList[current].size == size)
		{
			LinkedList[current].type = H;
			LinkedList[current].in_use = 0;
			LinkedList[current].arena = NULL;
			LinkedList[current].previous = index;
			LinkedList[current].size -= size;
		}

	}

	// if ret is zero, then no fail allocation, return index of node
	if (!ret)
	{
		return index;
	}
	// else error, return -1 ret value
	else
	{
		return ret;
	}
}

// this function uses passed in size to allocate memory using the best fit algorithm
int bestFit(int size)
{
    /** Index into the array where we will put this new node */
	int index    =  findFreeNodeInternal();   

    /** Set current to the rootNode so we start searching from the beginning */
	int current  =  -1;                 
	                                          
    /** Set previous to -1 since there nothing before the first node in the list */
	int previous = -1;                          
	                                          
    /** Our return value.  -1 on failure. 0 on success. */
	int ret      = -1; 

	// set best size to max possible values as smaller sized blocks are preferred
	int best_size = INT_MAX;                      
	

	if (index >= 0)
	{
		/*	loop through linked list until an appropriate block that is in use,
			large enough, and of type hole is found*/
		for (int i = 0; i < MAX_LINKED_LIST_SIZE; i++)
		{
			if (LinkedList[i].type == H && LinkedList[i].in_use && LinkedList[i].size >= size)
			{
				// if the remaining space in the block is lower than the previous best size...
				if((int)(LinkedList[i].size - size) < best_size)
				{
					// ... update best size and current,previous indexes
					best_size = LinkedList[i].size - size;
					current = i;
					previous = LinkedList[i].previous;	
				}
			}
		}

        // if the best size has not been updated (no eligible node found), return -1
		if (best_size == INT_MAX)
		{
			return -1;			
		}
        /** If we ran off the end of the list then insert on the tail of the list */
		else if( current == -1 )
		{
			ret = insertNodeInternal(LinkedList[previous].previous, index);
		}
		else
		{
 			ret = insertNodeInternal(previous, index);
 		}

	    /**
	     * Now that our new node is linked in lets set the size and mark this array element
	     * as being used, change type to process and update location in memory and next pointer
	     */
 		LinkedList[index].type = P;
		LinkedList[index].in_use = 1;
		LinkedList[index].size = size;
		LinkedList[index].arena = LinkedList[current].arena;
		LinkedList[index].next = current;
		LinkedList[index].previous = LinkedList[current].previous;

		// split node if leftover space in block
		if (LinkedList[current].size > size)
		{
			LinkedList[current].type = H;
			LinkedList[current].in_use = 1;
			LinkedList[current].arena += size;
			LinkedList[current].previous = index;
			LinkedList[current].size -= size;
		}
		// empty node if no leftover space
		else if (LinkedList[current].size == size)
		{
			LinkedList[current].type = H;
			LinkedList[current].in_use = 0;
			LinkedList[current].arena = NULL;
			LinkedList[current].previous = index;
			LinkedList[current].size -= size;
		}

	}

	// if ret is zero, then no fail allocation, return index of node
	// else error, return -1 ret value
	if (!ret)
	{
		return index;
	}
	else
	{
		return ret;
	}
}

int worstFit(int size)
{
    /** Index into the array where we will put this new node */
	int index    =  findFreeNodeInternal();   

    /** Set current to the rootNode so we start searching from the beginning */
	int current  =  -1;                 
	                                          
    /** Set previous to -1 since there nothing before the first node in the list */
	int previous = -1;                          
	                                          
    /** Our return value.  -1 on failure. 0 on success. */
	int ret      = -1; 

	// set worst size to min possible value as larger sized blocks are preferred
	int worst_size = INT_MIN;                      
	

	if (index >= 0)
	{
		/*	loop through linked list until an appropriate block that is in use,
			large enough, and of type hole is found*/
		for (int i = 0; i < MAX_LINKED_LIST_SIZE; i++)
		{
			if (LinkedList[i].type == H && LinkedList[i].in_use && LinkedList[i].size >= size)
			{
				// if the remaining space in the block is higher than the previous worst size...
				if((int)(LinkedList[i].size - size) > worst_size)
				{
					// ... update worst size and current,previous indexes
					worst_size = LinkedList[i].size - size;
					current = i;
					previous = LinkedList[i].previous;
				}
			}
		}

        // if the worst size has not been updated (no eligible node found), return -1
		if (worst_size == INT_MIN)
		{
			return -1;			
		}
        /** If we ran off the end of the list then insert on the tail of the list */
		else if( current == -1 )
		{
			ret = insertNodeInternal(LinkedList[previous].previous, index);
		}
		else
		{
 			ret = insertNodeInternal(previous, index);
 		}

	    /**
	     * Now that our new node is linked in lets set the size and mark this array element
	     * as being used, change type to process and update location in memory and next pointer
	     */
 		LinkedList[index].type = P;
		LinkedList[index].in_use = 1;
		LinkedList[index].size = size;
		LinkedList[index].arena = LinkedList[current].arena;
		LinkedList[index].next = current;
		LinkedList[index].previous = LinkedList[current].previous;

		// split node if leftover space in block
		if (LinkedList[current].size > size)
		{
			LinkedList[current].type = H;
			LinkedList[current].in_use = 1;
			LinkedList[current].arena += size;
			LinkedList[current].previous = index;
			LinkedList[current].size -= size;
		}
		// empty node if no leftover space
		else if (LinkedList[current].size == size)
		{
			LinkedList[current].type = H;
			LinkedList[current].in_use = 0;
			LinkedList[current].arena = NULL;
			LinkedList[current].previous = index;
			LinkedList[current].size -= size;
		}

	}

	// if ret is zero, then no fail allocation, return index of node
	// else error, return -1 ret value
	if (!ret)
	{
		return index;
	}
	else
	{
		return ret;
	}
}

int nextFit(int size)
{
    /** Index into the array where we will put this new node */
	int index = findFreeNodeInternal();

    /** Set current to the rootNode so we start searching from the beginning */
	int previous;

	// set global current to the rootnode for first run
	global_current_nextFit = rootNode;

	// set current to begin from the global current
	int current = global_current_nextFit;

	// if first allocation then set previous to default -1 value
	if (current == rootNode)
	{
		previous = -1;
	}
	// if not first allocation then previous to previous value of current
	else
	{
		previous = LinkedList[current].previous;
	}
	                                          
    /** Our return value.  -1 on failure. 0 on success. */
	int ret      = -1;                        
	

	if (index >= 0)
	{
      	/*	loop through linked list until an appropriate block that is in use,
			large enough, and of type hole*/
		while (current >= 0 &&
				!(LinkedList[current].type == H 	&&
			  	  LinkedList[current].in_use 	&&
			  	  LinkedList[current].size >= size))
		{
			// go to next node, update previous and current
			previous = current;
			current  = LinkedList[current].next;
		}

        /** If we ran off the end of the list then insert on the tail of the list */
		if( current == -1 )
		{
			// store return val, if -1 then fail
			ret = insertNodeInternal(LinkedList[previous].previous, index);
		}
		else
		{
			// store return val, if -1 then fail
 			ret = insertNodeInternal(previous, index);
 		}

	    /**
	     * Now that our new node is linked in lets set the size and mark this array element
	     * as being used, change type to process and update location in memory and next pointer
	     */
 		LinkedList[index].type = P;
		LinkedList[index].in_use = 1;
		LinkedList[index].size = size;
		LinkedList[index].arena = LinkedList[current].arena;
		LinkedList[index].next = current;

		// split node if leftover space in block
		if (LinkedList[current].size >= size)
		{
			LinkedList[current].type = H;
			LinkedList[current].in_use = 1;
			LinkedList[current].arena += size;
			LinkedList[current].previous = index;
			LinkedList[current].size -= size;
		}
		// empty node if no leftover space
		else if (LinkedList[current].size == size)
		{
			LinkedList[current].type = H;
			LinkedList[current].in_use = 0;
			LinkedList[current].arena = NULL;
			LinkedList[current].previous = index;
			LinkedList[current].size -= size;
		}

	}

	// update global current with new current value for next function call
	global_current_nextFit = current;

	// if ret is zero, then no fail allocation, return index of node
	// else error, return -1 ret value
	if (!ret)
	{
		return index;
	}
	else
	{
		return ret;
	}
}
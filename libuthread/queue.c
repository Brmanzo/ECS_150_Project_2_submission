#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"


#define QUEUE_SIZE 32

typedef struct node {
  void* value;
  struct node* next;
  struct node* prev;
}node_t;

struct queue {
  node_t* head;
  node_t* tail;
  node_t* iterator;
  int queue_size;
};

queue_t queue_create(void)
{
  /* Allocates space for an empty queue object if open space. */
  queue_t queue = (queue_t)malloc(QUEUE_SIZE);
  if (queue == NULL)
    return(NULL);
  /* Initializes object parameters. */
  queue->head = NULL;
  queue->tail = NULL;
  queue->iterator = NULL;
  queue->queue_size = 0;
  return(queue);
}

int queue_destroy(queue_t queue)
{
  if ((queue != NULL) && (queue_length(queue) == 0))
  {
    /* Deallocates memory of empty queue object. */
    free(queue);
    return(0);
  }
  else
    return(-1);
}

int queue_enqueue(queue_t queue, void* data)
{
  if ((queue != NULL) && (data != NULL))
  {
    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    if (new_node != NULL)
    {
      new_node->value = data;
      new_node->next = NULL;
      /* If the first node in queue, new node becomes head. */
	  if (queue->queue_size == 0)
	  {
        new_node->prev = NULL;
        queue->head = new_node;
      }
      /* Otherwise, the new node is enqueued at the tail. */
      else {
        new_node->prev = queue->tail;
        queue->tail->next = new_node;
      }
      queue->tail = new_node;
      /* Update queue size. */
      queue->queue_size++;
      return(0);
    }
    else
      return(-1);
  }
  else
    return(-1);
}

int queue_dequeue(queue_t queue, void** data)
{
  if ((queue != NULL) && (data != NULL) && (queue_length(queue) != 0))
  {
    if (queue->queue_size != 0)
    {
      /* Retrieves value of head and sets next to new head. */
      *data = queue->head->value;
      node_t* old_head = queue->head;
      queue->head = queue->head->next;
      free(old_head);
    }
    /* Update queue size. */
    queue->queue_size--;
    return(0);
  }
  else
    return(-1);
}

int queue_delete(queue_t queue, void* data)
{
  if ((queue != NULL) && (data != NULL))
  {
    queue->iterator = queue->head;
    while (queue->iterator != NULL)
    {
      if (queue->iterator->value == data)
      {
        /* If deleting head, redefine the next as new head. */
        if (queue->iterator == queue->head)
        {
          queue->head = queue->head->next;
        }
        /* If deleting tail, redefine prev as new tail and disconnect next. */
        else if(queue->iterator == queue->tail)
        {
          queue->tail = queue->tail->prev;
          queue->tail->next = NULL;
        }
        /* Otherwise connect prev with next, removing iterator from scope. */
        else
        {
          queue->iterator->prev->next = queue->iterator->next;
          queue->iterator->next->prev = queue->iterator->prev;
        }
        queue->queue_size--;
        return(0);
      }
      queue->iterator = queue->iterator->next;
    }
    return(-1);
  }
  else
    return(-1);
}

int queue_iterate(queue_t queue, queue_func_t func)
{
  if ((queue != NULL) && (func != NULL))
  {
    /* Start iterating at head. */
    queue->iterator = queue->head;
    while (queue->iterator != NULL)
    {
      /* Perform callback function on current node. */
      func(queue, queue->iterator->value);
      /* If callback function modifies queue, check next before iterating. */
      if (queue->iterator != NULL)
        queue->iterator = queue->iterator->next;
    }
    return(0);
  }
  else
    return(-1);
}

int queue_length(queue_t queue)
{
  if (queue != NULL)
    return(queue->queue_size);
  else
    return(-1);
}
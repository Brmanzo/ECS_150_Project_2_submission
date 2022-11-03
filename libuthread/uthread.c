#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

/* Enum to provide state information for TCB's.*/
enum {
  RUNNING,
  READY,
  BLOCKED,
  ZOMBIE
};

struct uthread_tcb {
  uthread_ctx_t* uctx;
  void* stack_ptr;
  int state;
};

/* Queue objects for library to handle. */
/* Ready queue handles threads that are ready to run. */
queue_t ready_queue;
/* Zombie queue handles threads that have exited and need to be deleted. */
queue_t zombie_queue;
/* Blocked queue handles threads that are blocked by their semaphores. */
queue_t blocked_queue;

/* Global TCB pointer to current thread outside of ready queue. */
struct uthread_tcb* current_thread;
/* Global TCB pointer to previous thread for use in uthread_yield(). */
struct uthread_tcb* previous_thread;


/*
 * uthread_ctx_t - User-level thread context
 *
 *This type is an opaque data structure type that contains a thread's execution
 * context.
 *
 * Such a context is initialized for the first time when creating a thread with
 * uthread_ctx_init(). Once initialized, it can be switched to with
 * uthread_ctx_switch().
 */
struct uthread_tcb* uthread_current(void)
{
  /* User function to retrieve the current thread. */
  return(current_thread);
}

void uthread_yield(void)
{
  /* Remembers current thread in temp variable previous_thread. */
  previous_thread = current_thread;
  previous_thread->state = READY;

  /* Enqueues previous_thread at tail of ready queue. */
  queue_enqueue(ready_queue, (void*)previous_thread);

  /* Dequeues new head into current_thread. */
  queue_dequeue(ready_queue, (void**)(&current_thread));
  current_thread->state = RUNNING;

  /* Switches context from old thread to new thread. */
  uthread_ctx_switch(previous_thread->uctx, current_thread->uctx);
}

void uthread_exit(void)
{
  /* Remembers current thread in temp variable previous_thread. */
  previous_thread = current_thread;
  previous_thread->state = ZOMBIE;

  /* Enqueue previous_thread at tail of zombie queue. */
  queue_enqueue(zombie_queue, (void*)previous_thread);

  /* Dequeues new head into current_thread. */
  queue_dequeue(ready_queue, (void**)(&current_thread));
  current_thread->state = RUNNING;

  /* Switches context from old thread to new thread. */
  uthread_ctx_switch(previous_thread->uctx, current_thread->uctx);
}

int uthread_create(uthread_func_t func, void* arg)
{
  /* Malloc for New_thread_TCB, create space for stack, and set state. */
  struct uthread_tcb* new_thread =
    (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
  new_thread->uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
  new_thread->stack_ptr = uthread_ctx_alloc_stack();
  new_thread->state = READY;
  if (!new_thread || !(new_thread->uctx) || !(new_thread->stack_ptr))
    return(-1);
  /* Initialize new context object (bootstrap() pauses before execution). */
  if (uthread_ctx_init(new_thread->uctx, new_thread->stack_ptr,
    func, arg) == -1)
  {
    return(-1);
  }

  /* New thread is enqueued at end of ready queue. */
  queue_enqueue(ready_queue, (void*)new_thread);
  return(0);
}

int uthread_run(bool preempt, uthread_func_t func, void* arg)
{
  /* REMOVE ME. */
  preempt = !preempt;
  /* Allocate space for all global queues to be handled by library. */
  ready_queue = queue_create();
  zombie_queue = queue_create();
  blocked_queue = queue_create();
  if ((!ready_queue) || (!zombie_queue) || (!blocked_queue))
    return(-1);

  /* Mallocing for current and previous thread global TCB pointers. */
  previous_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
  previous_thread->uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
  previous_thread->stack_ptr = uthread_ctx_alloc_stack();
  
  if (!previous_thread || !(previous_thread->uctx) || 
    !(previous_thread->stack_ptr))
  {
    return(-1);
  }
  current_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
  current_thread->uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
  current_thread->stack_ptr = uthread_ctx_alloc_stack();

  if (!current_thread || !(current_thread->uctx) ||
      !(current_thread->stack_ptr))
  {
      return(-1);
  }
  /* Malloc for Idle_thread_TCB, create space for stack, and set state*/
  struct uthread_tcb* idle_thread = 
      (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
  idle_thread->uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
  idle_thread->stack_ptr = uthread_ctx_alloc_stack();
  idle_thread->state = READY;

  if (!idle_thread || !(idle_thread->uctx) ||
      !(idle_thread->stack_ptr))
  {
      return(-1);
  }
  /* Idle thread initialized as current thread. */
  current_thread = idle_thread;

  /* Initial thread created and queued within ready queue. */
  if(uthread_create(func, arg) == -1)
      return(-1);

  /* The only time program returns to infinite loop will be when idle or deleting*/
  while (queue_length(ready_queue) > 0)
    uthread_yield();

  /* Frees allocated memory used by queues. */
  queue_iterate(ready_queue, (void*)queue_delete);
  queue_destroy(ready_queue);

  queue_iterate(zombie_queue, (void*)queue_delete);
  queue_destroy(zombie_queue);

  queue_iterate(blocked_queue, (void*)queue_delete);
  queue_destroy(blocked_queue);
  return(0);
}

void uthread_block(void)
{
  /* Initializes blocked thread from current_thread. */
  struct uthread_tcb* blocked_thread = current_thread;
  /* Sets blocked thread state and enqueues into blocked queue. */
  blocked_thread->state = BLOCKED;
  queue_enqueue(blocked_queue, (void*)blocked_thread);

  /* Reassigns current_thread as the next ready thread. */
  queue_dequeue(ready_queue, (void**)(&current_thread));
  current_thread->state = RUNNING;

  /* Switches context from blocked thread to new current thread*/
  uthread_ctx_switch(blocked_thread->uctx, current_thread->uctx);
}

void uthread_unblock(struct uthread_tcb* uthread)
{
  /* Deletes the newly readied thread from blocked queue. */
  queue_delete(blocked_queue, (void*)uthread);

  /* Enqueues the thread in the ready queue. */
  queue_enqueue(ready_queue, (void*)uthread);
  uthread->state = READY;
}



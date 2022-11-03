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

/* Enum to provide state information. */
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

/* Queue object for storing threads in. */
queue_t ready_queue;
queue_t zombie_queue;
queue_t blocked_queue;

/* TCB pointer to current thread when dequeued. */
struct uthread_tcb* previous_thread;
struct uthread_tcb* current_thread;

/*
 * uthread_ctx_t - User-level thread context
 *
 * This type is an opaque data structure type that contains a thread's execution
 * context.
 *
 * Such a context is initialized for the first time when creating a thread with
 * uthread_ctx_init(). Once initialized, it can be switched to with
 * uthread_ctx_switch().
 */
struct uthread_tcb* uthread_current(void)
{
	/* Nonredundant because for user calling. */
	return(current_thread);
}

void uthread_yield(void)
{
	/* Stores current thread in a temp variable. */
	previous_thread = current_thread;
	previous_thread->state = READY;

	/* Enqueue previous head at end of queue. */
	queue_enqueue(ready_queue, (void*)previous_thread);

	/* Dequeues new head into current_thread. */
	queue_dequeue(ready_queue, (void**)(&current_thread));
	current_thread->state = RUNNING;

	/* Switches context from old thread to new thread*/
	uthread_ctx_switch(previous_thread->uctx, current_thread->uctx);
}

void uthread_exit(void)
{
	/* Stores current thread in a temp variable. */
	previous_thread = current_thread;
	previous_thread->state = ZOMBIE;

	/* Enqueue previous head into zombie queue. */
	queue_enqueue(zombie_queue, (void*)previous_thread);

	/* Dequeues new head into current_thread. */
	queue_dequeue(ready_queue, (void**)(&current_thread));
	current_thread->state = RUNNING;

	/* Switches context from old thread to new thread*/
	uthread_ctx_switch(previous_thread->uctx, current_thread->uctx);
}

int uthread_create(uthread_func_t func, void* arg)
{
	/* Malloc for New_thread_TCB, create space for stack, and set state*/
	struct uthread_tcb* new_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	new_thread->uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	new_thread->stack_ptr = uthread_ctx_alloc_stack();
	new_thread->state = READY;

	uthread_ctx_init(new_thread->uctx, new_thread->stack_ptr, func, arg);

	queue_enqueue(ready_queue, (void*)new_thread);
	return(0);
}

int uthread_run(bool preempt, uthread_func_t func, void* arg)
{
	preempt = !preempt;
	/* Create queue at queue pointer(will exist for all future function calls) */
	ready_queue = queue_create();
	zombie_queue = queue_create();
	blocked_queue = queue_create();

	/* Mallocing for current and previous threads. */
	previous_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	previous_thread->uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	previous_thread->stack_ptr = uthread_ctx_alloc_stack();

	current_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	current_thread->uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	current_thread->stack_ptr = uthread_ctx_alloc_stack();

	/* Malloc for Idle_thread_TCB, create space for stack, and set state*/
	struct uthread_tcb* idle_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	idle_thread->uctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	idle_thread->stack_ptr = uthread_ctx_alloc_stack();
	idle_thread->state = READY;

	/* Idle Thread enqueued in queue. */
	current_thread = idle_thread;



	/* Creates Initial Thread */
	uthread_create(func, arg);

	/* The only time program returns to infinite loop will be when idle or deleting*/
	while (queue_length(ready_queue) > 0)
		uthread_yield();

	/* Frees allocated memory. */
	//queue_iterate(ready_queue, queue_delete);
	//queue_destroy(ready_queue);

	//queue_iterate(zombie_queue, queue_delete);
	//queue_destroy(zombie_queue);
	return(0);
}

void uthread_block(void)
{
	struct uthread_tcb* blocked_thread = uthread_current();
	blocked_thread->state = BLOCKED;
	queue_enqueue(blocked_queue, (void*)blocked_thread);

	queue_dequeue(ready_queue, (void**)(&current_thread));
	current_thread->state = RUNNING;

	/* Switches context from old thread to new thread*/
	uthread_ctx_switch(blocked_thread->uctx, current_thread->uctx);
}

void uthread_unblock(struct uthread_tcb* uthread)
{
	queue_delete(blocked_queue, (void*)uthread);

	queue_enqueue(ready_queue, (void*)uthread);
	uthread->state = READY;
}



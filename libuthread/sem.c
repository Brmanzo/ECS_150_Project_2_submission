#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	unsigned int count;
	queue_t waiting_queue;
};

sem_t sem_create(size_t count)
{
	sem_t sem = (sem_t)malloc(sizeof(struct semaphore));
	sem->count = count;
	sem->waiting_queue = queue_create();
	return sem;
}

int sem_destroy(sem_t sem)
{
	if ((sem == NULL) || (queue_length(sem->waiting_queue))) {
		return -1;
	}
	queue_iterate(sem->waiting_queue, (void*)queue_delete);
	queue_destroy(sem->waiting_queue);
	free(sem);
	return 0;	
}

int sem_down(sem_t sem)
{
	if (sem == NULL) {
		return -1;
	}
	while (sem->count == 0) {
		queue_enqueue(sem->waiting_queue, (void*)uthread_current());
		uthread_block();
	}
	sem->count--;
	return 0;
}

int sem_up(sem_t sem)
{
	if (sem == NULL) {
		return -1;
	}
	sem->count++;
	if ((queue_length(sem->waiting_queue)) > 0) {
		struct uthread_tcb* oldest_waiting_thread;
		queue_dequeue(sem->waiting_queue, (void**)&oldest_waiting_thread);
		uthread_unblock(oldest_waiting_thread);
	}
	return 0;
}

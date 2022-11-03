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
  /* Allocating and initializing semaphore pointer sem. */
  sem_t sem = (sem_t)malloc(sizeof(struct semaphore));
  sem->count = count;
  /* Assigns sem an individual waiting queue to prioritize blocked threads. */
  sem->waiting_queue = queue_create();
  return sem;
}

int sem_destroy(sem_t sem)
{
  if ((sem == NULL) || (queue_length(sem->waiting_queue)))
    return -1;
  /* Frees space used by elements of sem's waiting queue. */
  queue_iterate(sem->waiting_queue, (void*)queue_delete);
  /* Deletes waiting queue itself. */
  queue_destroy(sem->waiting_queue);
  free(sem);
  return 0;	
}

int sem_down(sem_t sem)
{
  if (sem == NULL)
    return -1;
  /* If sem has no resources threads are blocked and remembered by sem's  */
  /* waiting queue. While continues until there is an available resource. */
  while (sem->count == 0) {
    queue_enqueue(sem->waiting_queue, (void*)uthread_current());
    uthread_block();
  }
  /* Removes an available resource (requested resource is now being used). */
  sem->count--;
  return 0;
}

int sem_up(sem_t sem)
{
  if (sem == NULL)
    return -1;
  /* Provides an additional resource to sem. */
  sem->count++;
  /* Sends next blocked thread waiting for sem's resources to ready queue. */
  if ((queue_length(sem->waiting_queue)) > 0) {
    struct uthread_tcb* oldest_waiting_thread;
    queue_dequeue(sem->waiting_queue, (void**)&oldest_waiting_thread);
    /* Transfers thread from blocked queue to ready queue. */
    uthread_unblock(oldest_waiting_thread);
  }
  return 0;
}

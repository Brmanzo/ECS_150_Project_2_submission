Phase 1: queue API

The queue is required to be FIFO, and (almost) all operations must be O(1).
Based on this requirement, we decided to use a linked list, where the data
need not be recopied and transfered with every adjustment. This serves us
well as datapoints are stored in the same address space from initialization
until the program ends. Rather than doubly linked, we used a singly linked
list, as there is no need to traverse backwards. As we prioritize FIFO,
only the head is truly read from and removed. To implement this, we used a
standard series of node structs within a queue struct as taught in ECS36C.
Each node holds a void pointer to represent any possible given value. This
pointer does not need any extra memory allocated towards it, as the inputted
structures already exist when enqueued.

Phase 3: semaphore API

Initially, our semaphore structure contained solely an unsigned int. This was
to represent the amount of available resources in any given semaphore, which
would never fall below zero (this way, there are twice as many available
resources, assuming the request wants more than the int cap).
sem_create() and sem_destroy() are fairly straightforward with malloc() and
free(). sem_down() is designed to first check if a request should be blocked
before decrementing its resource. Inversing that process, sem_up() increments
its resource before unblocking.
In order to implement blocking and unblocking, we created a global queue in
parallel to the ready queue to offshore the designated threads. This way,
the uthread_yield() function only needs to concern itself with the next
available thread without constantly checking its status. This implementation
comes with a fault in that the general blocked queue doesn't remember the
necessary priority - the first thread to be unblocked is the one that waited
the longest, and is requesting the specific semaphor. uthread_unblock() is
also to receive a specific queue to unblock, rather than to figure out which
thread is most deserving. To alleviate this compatibility, we attach a queue
to each semaphor to represent blocked threads that are waiting on that
specific resource. With the combination of the general blocked queue and
respective semaphor waiting queue, we can safely retain the priority of threads
while saving a few cycles of runtime per thread yield.
uthread_block() takes the current_thread which called it, swaps placements
between blocked and ready queue, then context switches to run the new thread.
uthread_unblock() performs in the inverse order of block, but there is no
context switching. Unblock also deletes a specific tcb from within
blocked_queue, as its head is not guaranteed to be the requested thread.

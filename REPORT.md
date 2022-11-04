Authors: Bradley Manzo, Kent Cassidy
Date: November 3, 2022

#Report 2
    
In order to implement a user-level thread library for linux, we must
first construct a queue ADT to store threads in, develop a set of functions to
schedule said threads, a semaphore in order to manage resources usage, and a
signal alarm to enable preemption of lengthy threads as well as shield critical
sections of code. These separate components are facilitated by a static library
file to reduce compiling time and simplify dependencies between them.

##Phase 1: Queue API

The queue is required to be FIFO, and (almost) all operations must be O(1).
Based on this requirement, we decided to use a linked list, where the data
need not be recopied and transfered with every adjustment. This serves us
well as datapoints are stored in the same address space from initialization
until the program ends. Initially we had thought to use a singly linked
list, as there is no need to traverse backwards - as we prioritize FIFO,
usually only the head is truly read from and removed. We ended up creating
a doubly linked list - when deleting a node within any queue, the surrounding
elements must be rejoined. To do this, we need to keep track of the prev in
addition to next. To implement the linked list, we used a standard series of
node structs (holding value, prev, next) within a queue struct as taught in
ECS36C. Each node holds a void pointer to represent any possible given value.
This pointer does not need any extra memory allocated towards it, as the
inputted structures already exist when enqueued.
The makefile we wrote for the libuthread files works by compiling each .c
file into a .o, and then compiling all .o files into a .a static library. The
advantage of which being that all object files are linked together without
having to recompile every object file upon every single modification. Some rules
that we added include a generic rule for compiling .c files into .o files by
implementing a generic list of targets and objects. Another rule we added was a
clean rule similar to that of our p1, and a conditional variable to toggle the
-g and -O2 flags to choose either debugging or performance.

##Phase 2: Uthread API

The next step to implementing the thread library is to create a structure in
which to schedule and execute threads. This API greatly benefits from the above
Queue API, instantiating three queue objects: the ready, blocked, and zombie
queues to handle ready, blocked, and zombie threads respectively. These threads
are allocated space in memory with the idle thread, which is instantiated within
uthread_run(). The function then points a global pointer towards the idle thread
to exist outside of the ready queue and remembered across function calls, and to
be available to the user through uthread_current(). The initial thread is then
created and enqueued as the first element in the ready thread. Our uthread_
create function is responsible for allocating memory for each new thread and
initializing it using uthread_ctx_init(). This then makes the context, and the
uthread_ctx_bootstrap function pauses before executing and enqueues the thread
into the ready queue. Since all user created processes must be created within
this initial thread, the only thread returning to idle is idle itself. When idle
becomes the current thread, it yields to other threads in the queue unless
it is alone. Our uthread_yield function is called inside of idle and works by
storing the current_thread inside of a separate global TCB pointer previous
thread. This previous thread is then dequeued and enqueued in the ready queue.
The head of the ready queue is then dequeued into the current thread pointer.
This preserves the structure of the queue and ensures the current thread pointer
is always accurate. When a program completes its execution without yielding, it
is dequeued but is instead enqueued into our zombie queue to be destroyed upon
completion of uthread_run(). When idle is the only thread left in the ready
queue, it exists at current_thread and queue_length(ready_queue) is zero. This
is cause to exit the loop and destroy the leftover queues by passing queuedelete
into the queue_iterate function. Nailing the structure of this API was a
challenge as our first implementation required access to queue internals, eg.
queue->head->next->value which is unintended behavior for queue.c. Our next
attempt involved passing a custom get_head function to iterate and retrieving
the head this way, maintaining the current_thread within the head of ready
queue. However we settled on having current_thread exist outside as a global
pointer in order to preserve the integrity of the library and to minimize
potential errors involved with such a system.

##Phase 3: Semaphore API

Initially, our semaphore structure contained solely an unsigned int. This was
to represent the amount of available resources in any given semaphore, which
would never fall below zero (this way, there are twice as many available
resources, assuming the request wants more than the int cap).
sem_create() and sem_destroy() are fairly straightforward with malloc() and
free(). sem_down() is designed to first check if a request should be blocked
before decrementing its resource. Inversing that process, sem_up() increments
its resource before unblocking.
 
In order to implement blocking and unblocking, we created a global queue in
parallel to the ready queue to offshore the blocked threads. This way,
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

## Phase 4 Preemption

In order to enable preemption within out user-thread library, we attempted
to implement an alarm within a signal handler rather than blocking the signal
using a mask. With our system, we would've paused the timer when disabling
preempt, and restarted it by enabling preempts. However we were unable to get
the signal to work as intended. We have included our implementation, but despite
making a tester file, we cannot speak to the success of our preempt.c implement.

## Closing Remarks
While we were unable to complete phase 4, we were able to solidify our
understanding of linked lists, while building a greater understanding of
concepts like user-level threads and semaphores. In addition we learned how
static libraries can be compiled using intelligently designed makefiles.
While our preempt.c may not work, we believe that we understand the
importance of such a system on any thread scheduling system and after working
it out on paper, we still believe that our implementation could've worked.

#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include <private.h>
#include <uthread.h>


int thread2_count = 0;
int thread1_count = 0;

void thread2(void* arg)
{
  (void)arg;
  while (thread2_count < 100) {
    printf("\033[0;32mThread 2 is active\n\033[0m");
    thread2_count++;
    uthread_yield();
  }
}

void thread1(void* arg)
{
  (void)arg;

  uthread_create(thread2, NULL);
	
  (void)arg;
  while (thread1_count < 100) {
    sleep(1);
    printf("\033[0;31mThread 1 is spinning.\n\033[0m");
    thread1_count++;
  }
}

int main(void)
{
  uthread_create(thread1, NULL);
  uthread_run(false, thread1, NULL);
  preempt_start(true);
  preempt_stop();
  printf("thread1_count: %d, thread2_count: %d\n", thread1_count, thread2_count);
  return 0;
}

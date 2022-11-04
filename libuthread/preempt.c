#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

int disable_flag = 0; //GNU Lib Manual 24.7.7
struct sigaction sig_setup;
sigset_t block_mask;

struct itimerval timer_setup;

static void sig_handler() {
  if (disable_flag) {
    return;
  }
  else {
    setitimer(SIGVTALRM, &timer_setup, NULL);
    uthread_yield();
  }
}

void preempt_disable(void)
{
  disable_flag = 1;
}

void preempt_enable(void)
{
  disable_flag = 0;
  setitimer(SIGVTALRM, &timer_setup, NULL);
}

void preempt_start(bool preempt)
{	
  if (!preempt) {
    return;
  }
  else {
    sigemptyset(&block_mask);
    sig_setup.sa_handler = sig_handler;
    sig_setup.sa_mask = block_mask;
    /* If flag is set, returning from a handler resumes the */
    /* library function primitives (open, read, write).     */
    sig_setup.sa_flags = SA_RESTART;
    sigaction(SIGVTALRM, &sig_setup, NULL);

    /* u in usec stands for micro- */
    /* Period between successive timer attempts. If zero, the alarm will only be sent once. */
    timer_setup.it_interval.tv_sec = 0;
    timer_setup.it_interval.tv_usec = (1000000 / HZ);
    /* Period between now and first timer interrupt. If zero, the alarm is disabled. */
    timer_setup.it_value.tv_sec = 0;
    timer_setup.it_value.tv_usec = (1000000 / HZ);

    setitimer(ITIMER_VIRTUAL, &timer_setup, NULL);
  }
}

void preempt_stop(void)
{
  preempt_disable();
  signal(SIGVTALRM, SIG_DFL);
}

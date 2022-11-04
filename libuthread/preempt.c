#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

int disable_flag = 0; //GNU Lib Manual 24.7.7
struct sigaction sig_setup
sigset_t block_mask

void sig_handler () {
	if (disable_flag) {
		return;
	} else {
		alarm(1/HZ);
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
	alarm(1/HZ);
}

void preempt_start(bool preempt)
{
	if (!preempt) {
		return;
	} else {
		sigemptyset(&block_mask);
		sig_setup.sa_handler = sig_handler; //
		sig_setup.sa_mask = block_mask;
		sig_setup.sa_flags = 0;
		sigaction(SIGVTALRM, &sig_setup, NULL);
		alarm(1/HZ);
	}
}

void preempt_stop(void)
{
	preempt_disable();
	signal(SIGVTALRM, SIG_DFL);
}


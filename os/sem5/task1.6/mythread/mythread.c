#define _GNU_SOURCE
#include <sched.h>
#include "mythread.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/futex.h>
#include <linux/sched.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define STACK_SIZE (PAGE_SIZE * 10)
#define SUCCESS 0
#define FAILURE -1

/*
если *futex != val, то сразу возвращает управление,
если *futex == val, то поток блокируется, пока значение *futex не изменится и поток не будет пробуждён */
static int futex_wait(int *futexp, int val) {
	return syscall(SYS_futex, futexp, FUTEX_WAIT, val, NULL, NULL, 0);
}

static int futex_wake(int *futexp) {
	return syscall(SYS_futex, futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
}

static int start_routine_wrapper(void *thread_iter) {
	mythread_t thread = (mythread_t)thread_iter;
	void *(*start_routine)(void *) = thread->start_routine;
	void *arg = thread->arg;

	thread->retval = start_routine(arg);

	thread->finished = 1;
	thread->futex_finished_var = 1;
	futex_wake(&thread->futex_finished_var);

	if (thread->detached) {
		free(thread->stack);
		free(thread);
		return 0;
	}

	while (thread->joined == 0) {
		futex_wait(&thread->futex_joined_var, 0);
	}

	free(thread->stack);
	free(thread);
	return 0;
}

int mythread_create(mythread_t *thread_res, void *(*start_routine)(void *), void *arg) {
	void *stack = malloc(STACK_SIZE);
	if (stack == NULL) {
		fprintf(stderr, "Ошибка выделения памяти для скета потока\n");
		return FAILURE;
	}

	mythread_t thread = malloc(sizeof(mythread_struct_t));
	if (thread == NULL) {
		fprintf(stderr, "Ошибка выделения памяти для управляющей структуры потока\n");
		free(stack);
		return FAILURE;
	}
	thread->start_routine = start_routine;
	thread->arg = arg;
	thread->stack = stack;
	thread->finished = 0;
	thread->futex_finished_var = 0;
	thread->joined = 0;
	thread->joining_is_pending = 0;
	thread->futex_joined_var = 0;
	thread->detached = 0;

	thread->tid = clone(start_routine_wrapper, stack + STACK_SIZE, CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM, thread);
	if (thread->tid == -1) {
		perror("Ошибка clone при создании потока");
		free(stack);
		free(thread);
		return FAILURE;
	}

	*thread_res = thread;
	return SUCCESS;
}

int mythread_join(mythread_t thread, void **ret) {
	if (thread->joined) {
		return SUCCESS;
	}

	if (thread->joining_is_pending) {
		fprintf(stderr, "Ошибка при присоединении потока. Поток уже присоединяется к другому процессу\n");
		return FAILURE;
	}

	thread->joining_is_pending = 1;

	while (thread->finished == 0) {
		futex_wait(&thread->futex_finished_var, 0);
	}

	thread->joined = 1;
	thread->futex_joined_var = 1;
	futex_wake(&thread->futex_joined_var);

	return SUCCESS;
}

int mythread_detach(mythread_t thread) {
	thread->detached = 1;

	if (thread->finished) {
		thread->joined = 1;
		thread->futex_joined_var = 1;
		futex_wake(&thread->futex_joined_var);
	}

	return SUCCESS;
}

#include "queue.h"

#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

int max_count, reader_core, writer_core;

// gcc -pthread main.c queue-async.c -o q_async
// gcc -pthread main.c queue-spinlock.c -o q_spin
// gcc -pthread main.c queue-mutex.c -o q_mutex
// gcc -pthread main.c queue-condition.c -o q_cond
// gcc -pthread main.c queue-semaphore.c -o q_sem

// /usr/bin/time -v ./q_
// ./q_ 10000 0 0

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);	
	CPU_SET(n, &cpuset); 

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	// printf("set_cpu: set cpu %d\n", n);
}

void *reader(void *arg) {
	int expected = 0;
	queue_t *q = (queue_t *)arg;
	printf("reader:\t[%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(reader_core);

	while (1) {
		int val = -1;
		int ok = queue_get(q, &val);
		if (!ok)
			continue;

		if (expected != val) {
			printf(RED "ERROR: get value is %d but expected - %d" NOCOLOR "\n", val, expected);
			queue_print_stats(q);
		}

		expected = val + 1;
	}

	return NULL;
}

void *writer(void *arg) {
	int i = 0;
	queue_t *q = (queue_t *)arg;
	printf("writer:\t[%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(writer_core);

	while (1) {
		int ok = queue_add(q, i);
		if (!ok)
			continue;
		i++;
		usleep(100);
	}

	return NULL;
}

int main(int argc, char **argv) {
	if (argc < 4) {
		fprintf(stderr, "main: usage: ./prog.out <max_count> <reader_core> <writer_core>\n");
		return -1;
	}

	max_count = atoi(argv[1]);
	reader_core = atoi(argv[2]);
	writer_core = atoi(argv[3]);
	pthread_t reader_tid, writer_tid;
	queue_t *q;
	int err;
// инициализировать семафор чтобы выполнял свой функционал и экономил CPU
	printf("main:\t[%d %d %d]\n", getpid(), getppid(), gettid());

	q = queue_init(max_count);

	err = pthread_create(&reader_tid, NULL, reader, q);
	if (err) {
		fprintf(stderr, "main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	// sched_yield();

	err = pthread_create(&writer_tid, NULL, writer, q);
	if (err) {
		fprintf(stderr, "main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
	
	// queue_destroy(q);
	pthread_exit(NULL);
}

// top нагрузка CPU
// /usr/bin/time -v ./q_spin 1000 0 0 

// cas операция реализуется при помощи протокола, что кеш с атомарной переменной может трогать только 1 поток  
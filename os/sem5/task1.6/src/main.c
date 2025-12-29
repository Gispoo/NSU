#include "../mythread/mythread.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <unistd.h>

void *test_routine(void *arg) {
	printf("Thread:\t[%d, %ld]\n", getpid(), syscall(SYS_gettid));
	return NULL;
}

int main() {
	printf("Main:\t[%d, %ld]\n", getpid(), syscall(SYS_gettid));

	mythread_t thread;
	if (mythread_create(&thread, test_routine, NULL)) {
		fprintf(stderr, "Ошибка при создании потока\n");
		return 1;
	}

	// mythread_detach(thread);

	if (mythread_join(thread, NULL)) {
		fprintf(stderr, "Ошибка при присоединении потока\n");
		return 1;
	}
// цикл где 100 потоков создаются и join 
// 
	mythread_t t[100];
    for (int i = 0; i < 100; ++i) {
        if (mythread_create(&t[i], test_routine, NULL)) {
            fprintf(stderr, "Ошибка при создании потока %d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < 100; ++i) {
        if (mythread_join(t[i], NULL)) {
            fprintf(stderr, "Ошибка при присоединении потока %d\n", i);
            return 1;
        }
    }

	// sleep(5);
	return 0;
}
// cmake --build ./build
// ./build/bin/main.out
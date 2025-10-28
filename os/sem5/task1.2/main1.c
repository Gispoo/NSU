#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *my_thread(void *args) {
	int *res = malloc(sizeof(int));
	*res = 42;
	pthread_exit(res);
}

int main() {
	int err;
	pthread_t tid;
	int *res;

	err = pthread_create(&tid, NULL, my_thread, NULL);
	if (err)
	{
		fprintf(stderr, "ошибка при создании потока\n");
		return 1;
	}

	err = pthread_join(tid, (void **)&res);
	if (err)
	{
		fprintf(stderr, "ошибка при присоединении потока\n");
		return 1;
	}

	printf("res = %d\n", *res);
	free(res);
// как решить проблему передачи ответсвенности за отчистку памяти
// выделять вне потока и передавать ему указатель на выделенную память

	return 0;
}
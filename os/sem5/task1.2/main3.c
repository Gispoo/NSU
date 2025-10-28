#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *my_thread(void *args) {
	char **res = (char **)args;
	*res = malloc(sizeof(char) * 12);
	if (res == NULL) {
		fprintf(stderr, "Failed to allocate memory\n");
		return NULL;
	}
	strcpy(*res, "Hello World");
	return NULL;
}

int main() {
	int err;
	pthread_t tid;
	char *res;

	err = pthread_create(&tid, NULL, my_thread, &res);
	if (err) {
		fprintf(stderr, "ошибка при создании потока\n");
		return 1;
	}

	err = pthread_join(tid, NULL);
	if (err) {
		fprintf(stderr, "ошибка при присоединении потока\n");
		return 1;
	}

	printf("res = %s\n", res);
	free(res);
	return 0;
}
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cleanup(void *arg) {
	char *str = (char *)arg;
	free(str);
	printf("Память освобождена!");
}

void *my_thread(void *args) {
	char *str = (char *)malloc(13 * sizeof(char));
	if (str == NULL) {
		fprintf(stderr, "Failed to create thread");
		return NULL;
	}
	strcpy(str, "Привет");

	pthread_cleanup_push(cleanup, str); // Регистрация функции очистки памяти 

	while (1) {
		printf("%s\n", str);
	}
	// почему если закоменить pthread_cleanup_pop будет плохо, до сюда же не доходит выполнение
	// что такое макрос и как конкретно это работает. 
	pthread_cleanup_pop(1); // 1 - вызвать функцию очистки, 0 - не вызывать

	return NULL;
}

int main() {
	pthread_t tid;
	if (pthread_create(&tid, NULL, my_thread, NULL) != 0) {
		fprintf(stderr, "Failed to create thread");
		return 1;
	}

	sleep(0.1);
	pthread_cancel(tid);

	pthread_exit(NULL);
}
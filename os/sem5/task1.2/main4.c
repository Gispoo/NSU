#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *my_thread(void *args) {
	pthread_t thread_id = pthread_self();
	printf("thread_id = %lu\n", (unsigned long)thread_id);
	pthread_detach(thread_id);
	return NULL;
}

int main() {
	int threads_cnt = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	while (1) {
		pthread_t tid;
		if (pthread_create(&tid, NULL, my_thread, NULL) != 0) {
			printf("threads_cnt = %d\n", threads_cnt);
			fprintf(stderr, "Failed to create thread");
			return 1;
		}
		threads_cnt++;
	}
// можно ли получить возвращаемое значение у detached потока
// нет

// во что упирается кроме памяти
// ограничение кол-ва потоков
	pthread_attr_destroy(&attr);

	return 0;
}
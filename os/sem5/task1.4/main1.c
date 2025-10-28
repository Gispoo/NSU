#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *my_thread(void *args) {
	while (1) {
		printf("Привет\n");
	}
	return NULL;
}

int main() {
	pthread_t tid;
	if (pthread_create(&tid, NULL, my_thread, NULL) != 0) {
		fprintf(stderr, "Failed to create thread");
		return 1;
	}

	sleep(2); 
	pthread_cancel(tid);

	return 0;
}
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int cnt = 0;

void *my_thread(void *args) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	while (1) {
		cnt++;
		// pthread_testcancel();
	} 
	return NULL;
}

int main() {
	pthread_t tid;
	if (pthread_create(&tid, NULL, my_thread, NULL) != 0) {
		fprintf(stderr, "Failed to create thread");
		return 1;
	}

	sleep(1);
	pthread_cancel(tid);
	printf("cnt: %d\n", cnt);
	pthread_join(tid, NULL);

	return 0;
}
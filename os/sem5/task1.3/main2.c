#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
	int number;
	char *text;
} my_struct;

void *my_thread(void *args) {
	pthread_detach(pthread_self());
	my_struct *data = (my_struct *)args;
	printf("number = %d, text = %s\n", data->number, data->text);
	free(data->text);
	free(data);

	return NULL;
}

int main() {
	pthread_t thread;
	my_struct *data = malloc(sizeof(my_struct));
	if (data == NULL) {
		fprintf(stderr, "Failed to allocate memory\n");
		return 1;
	}

	printf("Size Привет %d\n", sizeof("Привет"));
	data->number = 54;
	data->text = malloc(7 * sizeof(char));
	if (data->text == NULL) {
		free(data);
		fprintf(stderr, "Failed to allocate memory\n");
		return 1;
	}
	strcpy(data->text, "Привет");

	if (pthread_create(&thread, NULL, my_thread, (void *)data) != 0) {
		perror("Failed to create thread");
		free(data->text);
		free(data);
		return 1;
	}

	pthread_exit(NULL);
}
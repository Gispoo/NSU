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
	my_struct *data = (my_struct *)args;
	printf("number = %d, text = %s\n", data->number, data->text);
	return NULL;
}

int main() {
	pthread_t thread;
	my_struct data;

	data.number = 54;
	data.text = malloc(7 * sizeof(char));
	if (data.text == NULL) {
		fprintf(stderr, "Failed to allocate memory\n");
		return 1;
	}
	strcpy(data.text, "Привет");

	if (pthread_create(&thread, NULL, my_thread, (void *)&data) != 0) {
		perror("Failed to create thread");
		free(data.text);
		return 1;
	}

	pthread_join(thread, NULL);
	free(data.text);
	printf("Память очищена\n");

	return 0;
}
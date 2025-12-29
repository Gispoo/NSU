#define _GNU_SOURCE
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_LENGTH 100 // Максимальная длина строки в ноде
int storage_size;			  // Кол-во нод в списке
int swap_chance;			  // Шанс обмена соседних нод

int inc_count = 0;		   // Кол-во нод, идущих по возрастанию
int inc_iters = 0;		   // Кол-во итераций счётчика нод, удущих по возрастанию
int dec_count = 0;		   // Кол-во нод, идущих по убыванию
int dec_iters = 0;		   // Кол-во итераций счётчика нод, удущих по убыванию
int eq_count = 0;		   // Кол-во одинаковых соседних нод
int eq_iters = 0;		   // Кол-во итераций счётчика одинаковых соседних нод
int swap_count = 0;		   // Кол-во свапов
atomic_int swap_iters = 0; // Кол-во итераций свапальщика

// Нода связного списка
typedef struct _node_t
{
	char val[MAX_STRING_LENGTH];
	struct _node_t *next;
	pthread_spinlock_t sync;
} node_t;

// Односвязный список
typedef struct _linked_list_t
{
	node_t *first;
} linked_list_t;

// Создание ноды
node_t *create_node(char *val)
{
	node_t *node = malloc(sizeof(node_t));

	strncpy(node->val, val, MAX_STRING_LENGTH);
	node->next = NULL;
	pthread_spin_init(&node->sync, PTHREAD_PROCESS_SHARED);

	return node;
}

// Уничтожение списка
void linked_list_destroy(linked_list_t *list)
{
	node_t *cur = list->first;
	while (cur->next != NULL)
	{
		node_t *tmp = cur->next;
		pthread_spin_destroy(&cur->sync);
		free(cur);
		cur = tmp;
	}
	free(cur);
	free(list);
}

// Счётчик нод, удущих по возрастанию
void *inc_routine(void *args)
{
	linked_list_t *list = (linked_list_t *)args;
	int inc_count_local;

	while (true)
	{
		pthread_spin_lock(&list->first->sync);
		node_t *prev = list->first;
		node_t *cur = prev->next;
		inc_count_local = 0;

		while (cur != NULL)
		{
			const int prev_size = strlen(prev->val);
			pthread_spin_unlock(&prev->sync);

			pthread_spin_lock(&cur->sync);
			const int cur_size = strlen(cur->val);

			if (prev_size < cur_size)
				++inc_count_local;

			prev = cur;
			cur = prev->next;
		}

		pthread_spin_unlock(&prev->sync);
		inc_count = inc_count_local;
		++inc_iters;
	}
}

// Счётчик нод, удущих по убыванию
void *dec_routine(void *args)
{
	linked_list_t *list = (linked_list_t *)args;
	int dec_count_local;

	while (true)
	{
		pthread_spin_lock(&list->first->sync);
		node_t *prev = list->first;
		node_t *cur = prev->next;
		dec_count_local = 0;

		while (cur != NULL)
		{
			const int prev_size = strlen(prev->val);
			pthread_spin_unlock(&prev->sync);

			pthread_spin_lock(&cur->sync);
			const int cur_size = strlen(cur->val);

			if (prev_size > cur_size)
				++dec_count_local;

			prev = cur;
			cur = prev->next;
		}

		pthread_spin_unlock(&prev->sync);
		dec_count = dec_count_local;
		++dec_iters;
	}
}

// Счётчик одинаковых соседних нод
void *eq_routine(void *args)
{
	linked_list_t *list = (linked_list_t *)args;
	int eq_count_local;

	while (true)
	{
		pthread_spin_lock(&list->first->sync);
		node_t *prev = list->first;
		node_t *cur = prev->next;
		eq_count_local = 0;

		while (cur != NULL)
		{
			const int prev_size = strlen(prev->val);
			pthread_spin_unlock(&prev->sync);

			pthread_spin_lock(&cur->sync);
			const int cur_size = strlen(cur->val);

			if (prev_size == cur_size)
				++eq_count_local;

			prev = cur;
			cur = prev->next;
		}

		pthread_spin_unlock(&prev->sync);
		eq_count = eq_count_local;
		++eq_iters;
	}
}

// Поток, меняющий ноды местами
void *swap_routine(void *args)
{
	linked_list_t *list = (linked_list_t *)args;

	while (true)
	{
		pthread_spin_lock(&list->first->sync);
		node_t *prev = list->first;
		node_t *cur;
		node_t *next;

		while (prev->next != NULL)
		{
			// Нужно ли переставлять соседние ноды
			if ((rand() % 100) > swap_chance)
			{
				cur = prev->next;
				pthread_spin_lock(&cur->sync);
				pthread_spin_unlock(&prev->sync);
				prev = cur;
				continue;
			}

			cur = prev->next;
			pthread_spin_lock(&cur->sync);

			next = cur->next;
			if (next == NULL)
			{
				pthread_spin_unlock(&cur->sync);
				break;
			}
			pthread_spin_lock(&next->sync);

			// Меняем местами cur и next
			prev->next = next;
			pthread_spin_unlock(&prev->sync);
			cur->next = next->next;
			pthread_spin_unlock(&cur->sync);
			next->next = cur;

			swap_count++;
			prev = next;
		}

		pthread_spin_unlock(&prev->sync);
		atomic_fetch_add(&swap_iters, 1);
	}
}

void *print_routine(void *args)
{
	printf("list-spinlock\n");
	printf("inc_iters\tdec_iters\teq_iters\tswap_iters\tsum\n");
	while (true)
	{
		sleep(1);
		printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n",
			   inc_iters, dec_iters, eq_iters, atomic_load(&swap_iters), inc_count + dec_count + eq_count);
	}
}

// Создание списка со случайными нодами
linked_list_t *create_linked_list()
{
	srand(time(NULL));
	linked_list_t *list = malloc(sizeof(linked_list_t));

	char str[MAX_STRING_LENGTH];
	memset(str, 0, MAX_STRING_LENGTH);
	int str_size = rand() % MAX_STRING_LENGTH;
	for (int i = 0; i < str_size; ++i)
		str[i] = '0' + (rand() % 10);

	list->first = create_node(str);

	node_t *last = list->first;
	for (int i = 1; i < storage_size; ++i)
	{
		memset(str, 0, MAX_STRING_LENGTH);
		str_size = rand() % MAX_STRING_LENGTH;
		for (int i = 0; i < str_size; ++i)
			str[i] = '0' + (rand() % 10);

		node_t *new_node = create_node(str);
		last->next = new_node;
		last = new_node;
	}

	return list;
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "main: usage: ./prog.out <storage_size> <swap_chance>\n");
		return -1;
	}
	storage_size = atoi(argv[1]);
	swap_chance = atoi(argv[2]);

	linked_list_t *list = create_linked_list();

	pthread_t threads[7];
	pthread_create(&threads[0], NULL, inc_routine, list);
	pthread_create(&threads[1], NULL, dec_routine, list);
	pthread_create(&threads[2], NULL, eq_routine, list);
	pthread_create(&threads[3], NULL, swap_routine, list);
	pthread_create(&threads[4], NULL, swap_routine, list);
	pthread_create(&threads[5], NULL, swap_routine, list);
	pthread_create(&threads[6], NULL, print_routine, NULL);

	pthread_exit(0);
}

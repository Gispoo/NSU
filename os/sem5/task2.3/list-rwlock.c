#define _GNU_SOURCE
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_STRING_LENGTH 100 // Максимальная длина строки в ноде
int storage_size;			  // Кол-во нод в списке
int swap_chance;			  // Шанс обмена соседних нод

// Важно: обычные int тут дают data-race (UB), потому что их пишут/читают разные потоки.
// Поэтому делаем счётчики атомарными.
atomic_int inc_count = 0;
atomic_int inc_iters = 0;
atomic_int dec_count = 0;
atomic_int dec_iters = 0;
atomic_int eq_count = 0;
atomic_int eq_iters = 0;
atomic_int swap_count = 0;
atomic_int swap_iters = 0;

static atomic_bool stop_flag = false;

typedef struct _node_t
{
	char val[MAX_STRING_LENGTH];
	struct _node_t *next;
	pthread_rwlock_t sync;
} node_t;

typedef struct _linked_list_t
{
	node_t *first;
} linked_list_t;

node_t *create_node(char *val)
{
	node_t *node = malloc(sizeof(node_t));

	strncpy(node->val, val, MAX_STRING_LENGTH);
	node->next = NULL;
	pthread_rwlock_init(&node->sync, NULL);

	return node;
}

void linked_list_destroy(linked_list_t *list)
{
	node_t *cur = list->first;
	while (cur->next != NULL)
	{
		node_t *tmp = cur->next;
		pthread_rwlock_destroy(&cur->sync);
		free(cur);
		cur = tmp;
	}
	free(cur);
	free(list);
}

// Счётчик нод, удущих по возрастанию
void *inc_routine(void *args)
{	
	printf("inc:\t[%d %d %d]\n", getpid(), getppid(), gettid());

	linked_list_t *list = (linked_list_t *)args;
	int inc_count_local;

	while (!atomic_load(&stop_flag))
	{
		pthread_rwlock_rdlock(&list->first->sync);
		node_t *prev = list->first;
		node_t *cur = prev->next;
		inc_count_local = 0;

		while (cur != NULL)
		{
			const int prev_size = strlen(prev->val);
			pthread_rwlock_unlock(&prev->sync);

			pthread_rwlock_rdlock(&cur->sync);
			const int cur_size = strlen(cur->val);

			if (prev_size < cur_size)
				++inc_count_local;

			prev = cur;
			cur = prev->next;
		}

		pthread_rwlock_unlock(&prev->sync);
		atomic_store(&inc_count, inc_count_local);
		atomic_fetch_add(&inc_iters, 1);
	}
	return NULL;
}

// Счётчик нод, удущих по убыванию
void *dec_routine(void *args)
{
	printf("dec:\t[%d %d %d]\n", getpid(), getppid(), gettid());
	linked_list_t *list = (linked_list_t *)args;
	int dec_count_local;

	while (!atomic_load(&stop_flag))
	{
		pthread_rwlock_rdlock(&list->first->sync);
		node_t *prev = list->first;
		node_t *cur = prev->next;
		dec_count_local = 0;

		while (cur != NULL)
		{
			const int prev_size = strlen(prev->val);
			pthread_rwlock_unlock(&prev->sync);

			pthread_rwlock_rdlock(&cur->sync);
			const int cur_size = strlen(cur->val);

			if (prev_size > cur_size)
				++dec_count_local;

			prev = cur;
			cur = prev->next;
		}

		pthread_rwlock_unlock(&prev->sync);
		atomic_store(&dec_count, dec_count_local);
		atomic_fetch_add(&dec_iters, 1);
	}
	return NULL;
}

// Счётчик одинаковых соседних нод
void *eq_routine(void *args)
{
	printf("eq:\t[%d %d %d]\n", getpid(), getppid(), gettid());
	linked_list_t *list = (linked_list_t *)args;
	int eq_count_local;

	while (!atomic_load(&stop_flag))
	{
		pthread_rwlock_rdlock(&list->first->sync);
		node_t *prev = list->first;
		node_t *cur = prev->next;
		eq_count_local = 0;

		while (cur != NULL)
		{
			const int prev_size = strlen(prev->val);
			pthread_rwlock_unlock(&prev->sync);

			pthread_rwlock_rdlock(&cur->sync);
			const int cur_size = strlen(cur->val);

			if (prev_size == cur_size)
				++eq_count_local;

			prev = cur;
			cur = prev->next;
		}

		pthread_rwlock_unlock(&prev->sync);
		atomic_store(&eq_count, eq_count_local);
		atomic_fetch_add(&eq_iters, 1);
	}
	return NULL;
}

void *swap_routine(void *args)
{
	printf("swap:\t[%d %d %d]\n", getpid(), getppid(), gettid());
	linked_list_t *list = (linked_list_t *)args;

	unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)gettid();
	while (!atomic_load(&stop_flag))
	{
		pthread_rwlock_wrlock(&list->first->sync);
		node_t *prev = list->first;
		node_t *cur;
		node_t *next;

		while (prev->next != NULL && !atomic_load(&stop_flag))
		{
			// swap_chance трактуем как вероятность свапа в процентах (0..100)
			if ((rand_r(&seed) % 100) >= swap_chance)
			{
				cur = prev->next;
				pthread_rwlock_wrlock(&cur->sync);
				pthread_rwlock_unlock(&prev->sync);
				prev = cur;
				continue;
			}

			cur = prev->next;
			pthread_rwlock_wrlock(&cur->sync);

			next = cur->next;
			if (next == NULL)
			{
				pthread_rwlock_unlock(&cur->sync);
				break;
			}
			pthread_rwlock_wrlock(&next->sync);

			// Меняем местами cur и next
			prev->next = next;
			pthread_rwlock_unlock(&prev->sync);
			cur->next = next->next;
			pthread_rwlock_unlock(&cur->sync);
			next->next = cur;

			atomic_fetch_add(&swap_count, 1);
			prev = next;
		}

		pthread_rwlock_unlock(&prev->sync);
		atomic_fetch_add(&swap_iters, 1);
	}
	return NULL;
}

void *print_routine(void *args)
{
	(void)args;
	printf("print:\t[%d %d %d]\n", getpid(), getppid(), gettid());
	printf("list-rwlock\n");
	printf("inc_iters\tdec_iters\teq_iters\tswap_iters\tsum\n");
	while (!atomic_load(&stop_flag))
	{
		sleep(1);
		printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n",
			   atomic_load(&inc_iters),
			   atomic_load(&dec_iters),
			   atomic_load(&eq_iters),
			   atomic_load(&swap_iters),
			   atomic_load(&inc_count) + atomic_load(&dec_count) + atomic_load(&eq_count));
	}
	return NULL;
}

// Создание списка со случайными нодами
linked_list_t *create_linked_list()
{
	unsigned int seed = (unsigned int)time(NULL);
	linked_list_t *list = malloc(sizeof(linked_list_t));

	char str[MAX_STRING_LENGTH];
	memset(str, 0, MAX_STRING_LENGTH);
	int str_size = (int)(rand_r(&seed) % MAX_STRING_LENGTH);
	for (int i = 0; i < str_size; ++i)
		str[i] = (char)('0' + (rand_r(&seed) % 10));

	list->first = create_node(str);

	node_t *last = list->first;
	for (int i = 1; i < storage_size; ++i)
	{
		memset(str, 0, MAX_STRING_LENGTH);
		str_size = (int)(rand_r(&seed) % MAX_STRING_LENGTH);
		for (int i = 0; i < str_size; ++i)
			str[i] = (char)('0' + (rand_r(&seed) % 10));

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
		fprintf(stderr, "usage: %s <storage_size> <swap_chance 0..100> [seconds=5] [quiet=0|1]\n", argv[0]);
		return -1;
	}
	storage_size = atoi(argv[1]);
	swap_chance = atoi(argv[2]);
	int seconds = (argc >= 4) ? atoi(argv[3]) : 5;
	int quiet = (argc >= 5) ? atoi(argv[4]) : 0;

	if (!quiet)
		printf("main:\t[%d %d %d]\n", getpid(), getppid(), gettid());

	linked_list_t *list = create_linked_list();

	pthread_t threads[7];
	pthread_create(&threads[0], NULL, inc_routine, list);
	pthread_create(&threads[1], NULL, dec_routine, list);
	pthread_create(&threads[2], NULL, eq_routine, list);
	pthread_create(&threads[3], NULL, swap_routine, list);
	pthread_create(&threads[4], NULL, swap_routine, list);
	pthread_create(&threads[5], NULL, swap_routine, list);
	if (!quiet)
		pthread_create(&threads[6], NULL, print_routine, NULL);
	else
		threads[6] = (pthread_t)0;

	sleep(seconds);
	atomic_store(&stop_flag, true);

	for (int i = 0; i < 6; ++i)
		pthread_join(threads[i], NULL);
	if (!quiet)
		pthread_join(threads[6], NULL);

	printf("RESULT storage=%d swap=%d sec=%d inc=%d dec=%d eq=%d swap_iters=%d swaps=%d\n",
	       storage_size, swap_chance, seconds,
	       atomic_load(&inc_iters),
	       atomic_load(&dec_iters),
	       atomic_load(&eq_iters),
	       atomic_load(&swap_iters),
	       atomic_load(&swap_count));

	linked_list_destroy(list);
	return 0;
}
// gcc -o list_rw list_rw.c -lpthread

// почему нельзя просто всегда нумеровать данные и захватывать в этом порядке, чтобы не было дедлоков
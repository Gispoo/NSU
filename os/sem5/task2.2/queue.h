#ifndef __FITOS_QUEUE_H__
#define __FITOS_QUEUE_H__

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _Queue queue_t; // Очередь

queue_t *queue_init(int max_count);	 // Инициализация очереди
void queue_destroy(queue_t *q);		 // Удаление очереди
int queue_add(queue_t *q, int val);	 // Добавление элемента в конец очереди
int queue_get(queue_t *q, int *val); // Достать первый элемент из очереди
void queue_print_stats(queue_t *q);	 // Напечатать статистику

#endif

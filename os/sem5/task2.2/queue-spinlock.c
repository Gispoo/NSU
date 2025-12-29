#include "queue.h"

#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>

typedef struct _QueueNode {
	int val;				
	struct _QueueNode *next;
} qnode_t;

struct _Queue {
	qnode_t *first;
	qnode_t *last;	
	int count;		
	int max_count;

	pthread_spinlock_t spinlock; 

	long add_attempts;		
	long get_attempts;	
	long add_count;		
	long get_count;		
	pthread_t qmonitor_tid;
};

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;

	printf("qmonit:\t[%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		sleep(1);
		
		queue_print_stats(q);
	}

	return NULL;
}

queue_t *queue_init(int max_count) {
	int err;

	queue_t *q = malloc(sizeof(queue_t));
	if (!q) {
		fprintf(stderr, "Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->count = 0;
	q->max_count = max_count;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err) {
		fprintf(stderr, "queue_init: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

	if (pthread_spin_init(&q->spinlock, PTHREAD_PROCESS_PRIVATE) != 0) {
		perror("queue_init: pthread_create() failed: Failed to initialize spinlock");
		abort();
	}

	return q;
}

void queue_destroy(queue_t *q) {
	pthread_cancel(q->qmonitor_tid);
	pthread_join(q->qmonitor_tid, NULL);
	pthread_spin_destroy(&q->spinlock);

	qnode_t *cur_node = q->first;
	while (cur_node != NULL) {
		qnode_t *tmp = cur_node;
		cur_node = cur_node->next;
		free(tmp);
	}
	free(q);
}

int queue_add(queue_t *q, int val) {
	pthread_spin_lock(&q->spinlock);

	q->add_attempts++;

	assert(q->count <= q->max_count);

	if (q->count == q->max_count) {
		pthread_spin_unlock(&q->spinlock);
		return 0;
	}

	qnode_t *new = malloc(sizeof(qnode_t));
	if (!new) {
		fprintf(stderr, "Cannot allocate memory for new node\n");
		abort();
	}
	new->val = val;
	new->next = NULL;

	if (!q->first)
		q->first = q->last = new;
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;
	q->add_count++;

	pthread_spin_unlock(&q->spinlock);
	return 1;
}

int queue_get(queue_t *q, int *val) {
	pthread_spin_lock(&q->spinlock);

	q->get_attempts++;

	assert(q->count >= 0);

	if (q->count == 0) {
		pthread_spin_unlock(&q->spinlock);
		return 0;
	}

	qnode_t *tmp = q->first;
	*val = tmp->val;

	q->first = q->first->next;
	if (!q->first)
		q->last = NULL;

	free(tmp);
	q->count--;
	q->get_count++;

	pthread_spin_unlock(&q->spinlock);
	return 1;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		   q->count,
		   q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		   q->add_count, q->get_count, q->add_count - q->get_count);
}

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *thread1_func(void *arg) {
	// Блокируем все сигналы
	sigset_t set;
	sigfillset(&set);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	while (1) {
		sleep(1);
	}
	return NULL;
}

void sigint_handler(int signo) {
	printf("2: Получен SIGINT(%d)\n", signo);
}

void *thread2_func(void *arg) {
	// Блокируем все сигналы кроме SIGINT
	sigset_t set;
	sigfillset(&set);
	sigdelset(&set, SIGINT);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	// Устанавливаем обработчик сигнала SIGINT
	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	sa.sa_flags = SA_RESTART; // Автоматически перезапускает прерванные системные вызовы.
	sigfillset(&sa.sa_mask);  // Блокирует все сигналы, при выполнении обработчика

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	while (1) {
		sleep(1);
	}
	return NULL;
}

void *thread3_func(void *arg) {
	// Блокируем все сигналы кроме SIGQUIT
	sigset_t set;
	sigfillset(&set);
	sigdelset(&set, SIGQUIT);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	sigset_t wait_set;
	sigemptyset(&wait_set);
	sigaddset(&wait_set, SIGQUIT);
	int sig;

	while (1) {
		if (sigwait(&wait_set, &sig)) {
			perror("sigwait");
			exit(1);
		}
		printf("3: Получен SIGQUIT(%d)\n", sig);
	}
	return NULL;
}

int main() {
	printf("Main: %d\n", getpid());

	// Блокируем все сигналы
	sigset_t set;
	sigfillset(&set);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	// Создаем потоки
	pthread_t thread1, thread2, thread3;
	pthread_create(&thread1, NULL, thread1_func, NULL);
	pthread_create(&thread2, NULL, thread2_func, NULL);
	pthread_create(&thread3, NULL, thread3_func, NULL);

	// Ожидаем завершения потоков
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

	return 0;
}
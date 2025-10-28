#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global = 0;

void *mythread(void *arg) {
	pthread_t self = pthread_self();
    pthread_t *created_tid = (pthread_t *)arg;
    int same = pthread_equal(self, *created_tid);

	printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
	printf("  pthread_self=%p  created_tid=%p, %d\n", (void*)self, (void*)*created_tid, same);

	int local_var = 1;
    static int local_static_var = 2;
    const int local_const_var = 3;

	int old_g = global;
    local_var++;
    global++;

	printf("  ADDRS: &local (%d)=%p  &local_static (%d)=%p  &local_const (%d)=%p  &global (%d)=%p\n", local_var,
        (void*)&local_var, local_static_var, (void*)&local_static_var, local_const_var, (void*)&local_const_var, global, (void*)&global);
	printf("  CHANGES: local=%d  global: %d -> %d\n\n", local_var, old_g, global);
	
	sleep(120);
	return NULL;
}

int main() {
	pthread_t tid[5];

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());
	printf("  &global (main) = %p\n\n", (void*)&global);

	for (long i = 0; i < 5; i++) {
        int err = pthread_create(&tid[i], NULL, mythread, &tid[i]);
        if (err) {
            fprintf(stderr, "pthread_create(%ld) failed: %s\n", i, strerror(err));
            return 1;
        }
    }
// проверить что если основной поток создал 2ой, 2ой 3ий и если 2ой умрет - умрет ли 3ий?
// зачем сторожевая страница (разделение между стеками потоков) - защита от залезания одного стека в другой при переполнении
	for (int i = 0; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }

	printf("main global=%d\n", global);

	return 0;
}
// как конкретно шедулер переключает выполнение потоков (какие ресурсы есть у потоков)
 
// зачем мы пишем какие то права в ммапе и в мпротекте. всё равно же можно по адресам писать
/*39099 mmap(NULL, 8392704, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, -1, 0 <unfinished ...>
39100 <... set_robust_list resumed>)    = 0
39099 <... mmap resumed>)               = 0x7bd10b3fe000
39100 rt_sigprocmask(SIG_SETMASK, [],  <unfinished ...>
39099 mprotect(0x7bd10b3ff000, 8388608, PROT_READ|PROT_WRITE <unfinished ...>
39100 <... rt_sigprocmask resumed>NULL, 8) = 0
39099 <... mprotect resumed>)           = 0
*/


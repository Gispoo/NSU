#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>

#define STACK_ALLOC_SIZE 1024 * 64    
#define HEAP_CHUNK_SIZE  (1 << 20)
#define MAP_PAGES        10 

static void *anon_region = NULL;

void sigsegv_handler(int signum, siginfo_t *info, void *context) {
    printf("Перехвачен SIGSEGV по адресу %p\n", info->si_addr);
    _exit(EXIT_FAILURE);
}

void eat_stack(int depth) {
    char buffer[STACK_ALLOC_SIZE];
    memset(buffer, depth, sizeof(buffer));
    if (depth > 0) {
        eat_stack(depth - 1);
    } else {
        printf("[Stack] Уровень рекурсии достиг нуля.\n");
    }
} 

int main() {
    int pid = getpid();
    printf("PID = %d\n", pid);
    getchar();
    sleep(3);

    printf("\n-- Растём стек через рекурсию --\n");
    eat_stack(5); 
    printf("Рост стека завершён. Смотрите maps.\n");
    getchar();

    printf("\n-- Выделяем в куче блоки по %d байт --\n", HEAP_CHUNK_SIZE);
    void **allocs = calloc(10, sizeof(void*));
    for (int i = 0; i < 5; ++i) {
        allocs[i] = malloc(HEAP_CHUNK_SIZE);
        printf("  Выделен блок %d по адресу %p\n", i, allocs[i]);
        sleep(1);
    }
    printf("Выделение в куче завершено. Смотрите maps.\n");
    getchar();

    printf("\n-- Освобождаем блоки --\n");
    for (int i = 0; i < 5; ++i) {
        free(allocs[i]);
        printf("  Блок %d освобождён\n", i);
        sleep(1);
    }
    sleep(5);
    free(allocs); // может ли brk приводить к проблемам при выделении больших кусков памяти 
    // при попытке освободить память в середине кучи

    size_t map_size = MAP_PAGES * sysconf(_SC_PAGESIZE);
    anon_region = mmap(NULL, map_size,
                       PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_PRIVATE,
                       -1, 0);
    if (anon_region == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("\n-- Анонимный регион %zu байт по адресу %p создан --\n",
           map_size, anon_region);
    getchar();

    printf("\n-- Убираем PROT_READ и пробуем читать --\n");
    if (mprotect(anon_region, map_size, PROT_WRITE) == -1) {
        perror("mprotect");
    }
    struct sigaction sa = { .sa_sigaction = sigsegv_handler,
                            .sa_flags = SA_SIGINFO };
    sigaction(SIGSEGV, &sa, NULL);

    printf("Пытаемся читать из региона...\n");
    volatile char x;
    x = *((char*)anon_region);

    printf("\n-- Убираем PROT_WRITE и пробуем писать --\n");
    if (mprotect(anon_region, map_size, PROT_READ) == -1) {
        perror("mprotect");
    }

    printf("Пытаемся писать в регион...\n"); // почему ОС не может предоставить гарантии что флаги прав работает и почему зависит от архитектуры процессоров
    *((char*)anon_region) = 'Z'; // ОС только настраивает таблицы страниц. проверкой прав и преобразованием VR адресов в физ память занимается MMU
    printf("Запись успешна.\n");
    getchar();

    printf("\n-- Удаляем страницы 4-6 --\n");
    void *start = anon_region + 4 * sysconf(_SC_PAGESIZE);
    size_t length = 3 * sysconf(_SC_PAGESIZE);
    if (munmap(start, length) == -1) {
        perror("munmap");
    } else {
        printf("Страницы 4-6 удалены.\n");
    }
    printf("Окончательный просмотр maps.\n");
    getchar();

    munmap(anon_region, map_size);
    return 0;
}

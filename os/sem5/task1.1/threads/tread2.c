#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* thread3_func(void* arg) {
    printf("Поток3: Запущен (ID: %lu)\n", pthread_self());
    sleep(5);
    printf("Поток3: Завершается\n");
    return NULL;
}

void* thread2_func(void* arg) {
    printf("Поток2: Запущен (ID: %lu)\n", pthread_self());
    
    pthread_t thread3;
    pthread_create(&thread3, NULL, thread3_func, NULL);
    printf("Поток2: Создал поток3\n");
    
    sleep(1);
    printf("Поток2: Завершается (pthread_exit)\n");
    pthread_exit(NULL);
    return NULL;
}

int main() {
    pthread_t thread2;
    pthread_create(&thread2, NULL, thread2_func, NULL);
    printf("Основной: Создал поток2\n");
    
    sleep(7); 
    printf("Основной: Завершается\n");
    return 0;
}
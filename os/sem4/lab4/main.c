#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int global_initialized = 10;
int global_uninitialized;

const int global_constant = 20;

void print_addresses() {
  int local_variable = 30;
  printf("Адрес локальной переменной (stak): %p\n", (void*)&local_variable);

  static int static_variable = 40;
  printf("Адрес статической переменной (data): %p\n", (void*)&static_variable);

  const int local_constant = 50;
  printf("Адрес константы в функции (stak): %p\n", (void*)&local_constant);

  printf("Адрес глобальной инициализированной переменной (.data): %p\n", (void*)&global_initialized);
  printf("Адрес глобальной неинициализированной переменной (.bss): %p\n", (void*)&global_uninitialized);
  printf("Адрес глобальной константы (.rodata): %p\n", (void*)&global_constant);
}

int* return_local_address() {
  int local = 60;
  int* ptr = &local;
  return ptr;
}

void heap_operations() {
  char* buffer1 = (char*)malloc(100);
  if (buffer1 == NULL) {
    perror("Ошибка выделения памяти");
    return;
  }
  strcpy(buffer1, "hello world");

  printf("Содержимое буфера 1: %s\n", buffer1);

  free(buffer1);
  // как устроена куча 
  // что конкретно происходит при free. возможно неверен эксперимент
  printf("Содержимое буфера 1 после освобождения (неопределенное поведение): %s\n", buffer1);
  // попробовать вывести не принтом.

  char* buffer2 = (char*)malloc(100);
  if (buffer2 == NULL) {
    perror("Ошибка выделения памяти");
    return;
  }

  strcpy(buffer2, "hello world");

  printf("Содержимое буфера 2: %s\n", buffer2);

  char* middle_ptr = buffer2 + 50;
  free(middle_ptr); // как free определяет что указатель не начало памяти 

  printf("Содержимое буфера 2 после некорректного освобождения (неопределенное поведение): %s\n", buffer2);
    free(buffer2);
}

int main() {
  print_addresses();

  int* dangling_pointer = return_local_address();
  printf("Адрес, возвращенный из функции: %p\n", (void*)dangling_pointer);

  // heap_operations();

  char* env_value = getenv("MY_ENV_VAR");
  if (env_value != NULL) {
    printf("Значение переменной окружения до изменения: %s\n%p", env_value, &env_value);
  } else {
    printf("Переменная окружения MY_ENV_VAR не найдена.\n");
  }
// 
  setenv("MY_ENV_VAR", "changed value", 1); // 1 - перезаписывать существующую переменную

  env_value = getenv("MY_ENV_VAR");
  if (env_value != NULL) {
    printf("Значение переменной окружения после изменения: %s\n", env_value);
  } else {
    printf("Переменная окружения MY_ENV_VAR не найдена.\n");
  }

  // export MY_ENV_VAR="test"
  // ./my_program
  // echo $MY_ENV_VAR


  printf("PID %d\n", getpid());
  getchar();
  return 0;
}

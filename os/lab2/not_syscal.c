#include <unistd.h>

void _start() {
    const char msg[] = "Hello, World!\n";

    asm volatile (
        "movq $1, %%rax\n\t"        // syscall номер 1 — write
        "movq $1, %%rdi\n\t"        // дескриптор 1 — stdout
        "movq %0, %%rsi\n\t"        // адрес строки
        "movq $14, %%rdx\n\t"       // длина строки (в байтах)
        "syscall\n\t"
        :
        : "r"(msg)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
    
    // Возвращаем 0 (стандартное завершение программы)
    asm volatile ("mov $60, %rax\n\t"   // syscall номер 60 — exit
                  "xor %rdi, %rdi\n\t" // статус 0
                  "syscall\n\t");
}
// 







// что будет плохого если удалим 14 строку
// почему syscall не принимает адрес в отличие от call
// что содержит в себе программа
// может ли программа контролировать ограничения
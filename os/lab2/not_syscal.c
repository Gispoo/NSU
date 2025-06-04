#include <unistd.h>

void _start() {
    const char msg[] = "Hello, World!\n";

    asm volatile (
        "movq $1, %%rax\n\t"        // syscall ����� 1 � write
        "movq $1, %%rdi\n\t"        // ���������� 1 � stdout
        "movq %0, %%rsi\n\t"        // ����� ������
        "movq $14, %%rdx\n\t"       // ����� ������ (� ������)
        "syscall\n\t"
        :
        : "r"(msg)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
    
    // ���������� 0 (����������� ���������� ���������)
    asm volatile ("mov $60, %rax\n\t"   // syscall ����� 60 � exit
                  "xor %rdi, %rdi\n\t" // ������ 0
                  "syscall\n\t");
}
// 







// ��� ����� ������� ���� ������ 14 ������
// ������ syscall �� ��������� ����� � ������� �� call
// ��� �������� � ���� ���������
// ����� �� ��������� �������������� �����������
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global_var = 100;

// ps -elf | grep main
// ps aux

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        
        pid_t pid2 = fork();
        if (pid2 == 0) {
            // === ��� ������� ===
            printf("child %d\n", getppid());
            sleep(10);
            printf("child %d\n", getppid());
            sleep(10);
        } else {
            // === ���� ===
            printf("dad %d\n", getpid());
            exit(5);
        }
    } else {
        // === ��� ������� ===
        printf("Grdad %d\n", getpid());
        sleep(30);

        int status;
        getchar();
        waitpid(pid, &status, 0);

    }
    return 0;
}

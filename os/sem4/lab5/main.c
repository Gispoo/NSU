#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int global_var = 100;

// ps -elf | grep main
// ps aux

int main() {
    int local_var = 200;

    printf("==[Parent process]==\n");
    printf("PID: %d\n", getpid());
    printf("Address of global_var: %p, value: %d\n", (void*)&global_var, global_var);
    printf("Address of local_var: %p, value: %d\n", (void*)&local_var, local_var);

    pid_t pid = fork();

    if (pid == 0) { // как процессор создает прерываение, но он не вкурсе о COW, а прерывание нужно
        // форк помечает память недоступной для записи, процессор пытается записаться, вызывается исключение, ОС это обрабатывает
        // === Дочерний процесс ===
        printf("\n==[Child process]==\n");
        printf("PID: %d, Parent PID: %d\n", getpid(), getppid());
        printf("Address of global_var: %p, value: %d\n", (void*)&global_var, global_var);
        printf("Address of local_var: %p, value: %d\n", (void*)&local_var, local_var);
        
        // *(char*) ((void*)&global_var - 100) = 15;

        getchar();
        // raise(SIGSTOP);
        // sleep(20);
        global_var = 500; 
        local_var = 600;
        printf("Modified global_var: %d\n", global_var);
        printf("Modified local_var: %d\n", local_var);

        exit(5);
    } else {
        // === Родительский процесс ===
        getchar();

        printf("\n==[Parent after fork]==\n");
        printf("global_var: %d\n", global_var);
        printf("local_var: %d\n", local_var);

        int status;
        getchar();
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) { // чего ждет wait (завершение неверно)
            // ждем код завершения дочернего процесса.  
            printf("Child exited normally with code %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child terminated by signal %d\n", WTERMSIG(status));
        } else {
            printf("Child exited abnormally----------\n");
        }
    }
    
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--child") == 0) {
        printf("Hello world (PID=%d) after exec()\n", getpid());
        getchar();
        return 0;
    }

    int pid = getpid();
    printf("PID = %d\n", pid);

    getchar();
    int b = 10;
    mmap(); 
    // как пон€ть сохран€ютс€ ли файловые дискрипторы
    // дескрипторы лежат в task struckt. почитать про него. 

    char *newargv[] = { argv[0], "--child", NULL };
    if (execv(argv[0], newargv) == -1) {
        perror("execv");
        exit(EXIT_FAILURE);
    }

    return 0;
}
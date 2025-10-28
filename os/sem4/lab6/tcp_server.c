#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 8888
#define BUF_SIZE 1024

void handle_client(int client_fd) {
    char buffer[BUF_SIZE];
    ssize_t n;

    while ((n = read(client_fd, buffer, BUF_SIZE)) > 0) {
        buffer[n] = '\0';
        printf("Сообщение от клиента (fd %d): %s", client_fd, buffer);
        write(client_fd, buffer, n);
    }

    printf("Клиент (fd %d) отключился\n", client_fd);
    close(client_fd);
    exit(0);
}


void reap_zombie(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    int listen_fd, client_fd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    signal(SIGCHLD, reap_zombie); // Удаляем зомби-процессы

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    } 

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT); // зачем htons? почему нельзя просто число
    // запись порта задом наперед

    if (bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    listen(listen_fd, 5);
    printf("TCP Echo Server запущен на порту %d\n", PORT);

    while (1) {
        client_fd = accept(listen_fd, (struct sockaddr *)&cliaddr, &clilen);
        if (client_fd < 0) continue;

        printf("Новое подключение от %s:%d\n",
               inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

        pid_t pid = fork(); // как без fork. какая разница если 1 ядро, почему с 1 ядром всё равно норм робит
        if (pid == 0) { // имитация параллельности на 1ом процессе с помощью шедулера (распределяет цпу квоту)
            close(listen_fd);
            handle_client(client_fd);
        } else {
            close(client_fd);
        }
    }
}

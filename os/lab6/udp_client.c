#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Введите сообщение: ");
        if (fgets(buffer, BUF_SIZE, stdin) == NULL) {
            perror("fgets");
            break;
        }

        // Удаление символа новой строки, если есть
        buffer[strcspn(buffer, "\n")] = '\0';

        ssize_t sent = sendto(sockfd, buffer, strlen(buffer), 0,
                              (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (sent < 0) {
            perror("sendto");
            continue;
        }

        socklen_t len = sizeof(servaddr);
        ssize_t received = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0,
                                    (struct sockaddr *)&servaddr, &len);
        if (received < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[received] = '\0';
        printf("Ответ сервера: %s\n", buffer);

        // Завершение по команде
        if (strcmp(buffer, "exit") == 0) break;
    }

    close(sockfd);
    return 0;
}

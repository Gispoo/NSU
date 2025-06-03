#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET; // ip4
    servaddr.sin_addr.s_addr = INADDR_ANY; // слушать все интерфейсы
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Echo Server запущен на порту %d\n", PORT);

    while (1) {
        socklen_t len = sizeof(cliaddr);
        ssize_t n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) continue;

        buffer[n] = '\0';
        printf("Получено от %s:%d: %s\n",
               inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), buffer);

        sendto(sockfd, buffer, n, 0, (struct sockaddr *)&cliaddr, len);
    }
}

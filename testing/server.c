#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../src/game_arch.h"

// #define BUFFER_SIZE 255

int main() {
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in servaddr = {0};

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("failed to create server socket");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_port = htons(12345);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int rc = bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr));

    if (rc == -1) {
        perror("failed to bing");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    socklen_t len = 0;

    while (1) {
        int n = recvfrom(sockfd, (char*)buffer, BUFFER_SIZE, MSG_WAITALL, 0, &len);
        buffer[n] = '\0';
        printf("%s", buffer);
        printf("\n");
    }

    printf("закрили сервер\n");
    close(sockfd);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>

#include "game_arch.h"

// #define PORT 12345

// char* format_str(char* str, ...) {
//     va_list args;
//     va_start(args, )
// }

int udp_log(char* format, ...) {

    va_list args;
    va_start(args, format);
    char msg[BUFFER_SIZE];
    vsprintf(msg, format, args);
    va_end(args);

    // char* msg = "msg from client";
    struct sockaddr_in servaddr = {0};
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("failed to create client socket");
        return -1;
        // exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // INADDR_ANY; // 0.0.0.0 ?

    int len = sendto(sockfd, (const char*)msg, strlen(msg) /*BUFFER_SIZE*/, 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));

    // printf("strlen(msg) = %ld", strlen(msg));

    return 0;
}

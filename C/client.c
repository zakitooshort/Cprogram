#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE], response[BUFFER_SIZE];

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Connected to the Management Server. Type 'exit' to quit.\n");

    while (1) {
        printf("\nEnter word and target language (arabic/french): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        send(sock, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "exit") == 0) break;

        memset(response, 0, BUFFER_SIZE);
        recv(sock, response, BUFFER_SIZE, 0);
        printf("%s\n", response);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
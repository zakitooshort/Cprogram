#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

#define ARABIC_SERVER_PORT 8081
#define FRENCH_SERVER_PORT 8082

char* forward_request(const char* word, const char* target_language) {
    int server_port = (strcmp(target_language, "arabic") == 0) ? ARABIC_SERVER_PORT : FRENCH_SERVER_PORT;

    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char response[BUFFER_SIZE];
    static char result[BUFFER_SIZE];

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    send(sock, word, strlen(word), 0);
    recv(sock, response, BUFFER_SIZE, 0);

    snprintf(result, BUFFER_SIZE, "%s", response);
    closesocket(sock);
    WSACleanup();
    return result;
}

void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE], word[BUFFER_SIZE], lang[BUFFER_SIZE], response[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (strcmp(buffer, "exit") == 0) break;

        sscanf(buffer, "%s %s", word, lang); // Parse input
        char *translation = forward_request(word, lang);
        snprintf(response, BUFFER_SIZE, "Translation: %s", translation);
        send(client_socket, response, strlen(response), 0);
    }

    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    WSAStartup(MAKEWORD(2, 2), &wsa);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    printf("Management Server running on port %d...\n", PORT);

    while ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len)) != INVALID_SOCKET) {
        handle_client(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
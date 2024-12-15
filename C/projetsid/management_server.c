#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h> 

#define MANAGEMENT_PORT 5000
#define BUFFER_SIZE 1024
#define ARABIC_SERVER_PORT 5001
#define FRENCH_SERVER_PORT 5002

void handle_client(void *client_socket) {
    SOCKET client_sock = *(SOCKET *)client_socket;
    free(client_socket);

    char buffer[BUFFER_SIZE], word[BUFFER_SIZE], language[BUFFER_SIZE];
    char *token;
    int target_port;
    struct sockaddr_in translation_server_addr;
    SOCKET translation_sock;

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_sock, buffer, BUFFER_SIZE, 0);

    // Parse the request
    token = strtok(buffer, "|");
    if (token != NULL) {
        strncpy(word, token, BUFFER_SIZE);
        token = strtok(NULL, "|");
        if (token != NULL) {
            strncpy(language, token, BUFFER_SIZE);

            
            if (strcmp(language, "AR") == 0) {
                target_port = ARABIC_SERVER_PORT;
            } else if (strcmp(language, "FR") == 0) {
                target_port = FRENCH_SERVER_PORT;
            } else {
                send(client_sock, "Invalid language.", strlen("Invalid language."), 0);
                closesocket(client_sock);
                return;
            }

            
            translation_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (translation_sock == INVALID_SOCKET) {
                printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
                exit(EXIT_FAILURE);
            }

            translation_server_addr.sin_family = AF_INET;
            translation_server_addr.sin_port = htons(target_port);
            translation_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            if (connect(translation_sock, (struct sockaddr *)&translation_server_addr, sizeof(translation_server_addr)) < 0) {
                printf("Connection to translation server failed. Error Code: %d\n", WSAGetLastError());
                closesocket(translation_sock);
                closesocket(client_sock);
                return;
            }

            send(translation_sock, word, strlen(word), 0);
            memset(buffer, 0, BUFFER_SIZE);
            recv(translation_sock, buffer, BUFFER_SIZE, 0);
            send(client_sock, buffer, strlen(buffer), 0);

            closesocket(translation_sock);
        }
    }

    closesocket(client_sock);
}

int main() {
    WSADATA wsa;
    SOCKET server_sock, client_sock, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size = sizeof(client_addr);

    
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(MANAGEMENT_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    
    if (listen(server_sock, 5) < 0) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("Management server is running...\n");

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size)) != INVALID_SOCKET) {
        new_sock = malloc(sizeof(SOCKET));
        *new_sock = client_sock;
        _beginthread(handle_client, 0, (void *)new_sock);
    }

    closesocket(server_sock);
    WSACleanup();
    return 0;
}
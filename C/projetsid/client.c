#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_PORT 5000
#define BUFFER_SIZE 1024

void handle_client() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char word[BUFFER_SIZE], language[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection to management server failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter one of these (hello,world,computer,programming,language) (or 'exit' to quit) : \n");
        fgets(word, BUFFER_SIZE, stdin);
        word[strcspn(word, "\n")] = 0; // Remove newline
        if (strcmp(word, "exit") == 0) {
            break;
        }

        printf("Enter target language (AR/FR): ");
        fgets(language, BUFFER_SIZE, stdin);
        language[strcspn(language, "\n")] = 0;

        snprintf(buffer, BUFFER_SIZE, "%s|%s", word, language);

        // Send request to server
        send(sock, buffer, strlen(buffer), 0);

        // Receive response
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, BUFFER_SIZE, 0);
        printf("Response: %s\n", buffer);
    }

    closesocket(sock);
    WSACleanup();
}

int main() {
    handle_client();
    return 0;
}
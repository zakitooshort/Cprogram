#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define FRENCH_SERVER_PORT 5002
#define BUFFER_SIZE 1024


const char *dictionary[][2] = {
    {"hello", "Coucou"},
    {"goodmorning", "Bonjour"},
    {"goodnight", "Bonuit"},
    {"cat", "chat"},
    {"dog", "chien"},
    {NULL, NULL} 
};


const char *translate_to_french(const char *word) {
    for (int i = 0; dictionary[i][0] != NULL; i++) {
        if (strcmp(dictionary[i][0], word) == 0) {
            return dictionary[i][1];
        }
    }
    return "Translation not found.";
}

int main() {
    WSADATA wsa;
    SOCKET server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(FRENCH_SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    
    if (listen(server_sock, 5) < 0) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("French Translation Server is running...\n");

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size)) != INVALID_SOCKET) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_sock, buffer, BUFFER_SIZE, 0);

        const char *response = translate_to_french(buffer);
        send(client_sock, response, strlen(response), 0);

        closesocket(client_sock);
    }

    closesocket(server_sock);
    WSACleanup();
    return 0;
}
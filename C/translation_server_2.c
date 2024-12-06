#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8082 // Change port for French translation
#define BUFFER_SIZE 1024

void handle_translation(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // English → French dictionary
    char *dictionary[][2] = {
        {"hello", "bonjour"},
        {"world", "monde"},
        {"goodbye", "au revoir"},
        {"please", "s'il vous plaît"},
        {"thank you", "merci"},
        {"yes", "oui"},
        {"no", "non"},
        {NULL, NULL} // End of dictionary
    };

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (strcmp(buffer, "exit") == 0) break;

        int found = 0;
        for (int i = 0; dictionary[i][0] != NULL; i++) {
            if (strcmp(buffer, dictionary[i][0]) == 0) {
                snprintf(response, BUFFER_SIZE, "%s", dictionary[i][1]);
                found = 1;
                break;
            }
        }

        if (!found) snprintf(response, BUFFER_SIZE, "Word not found");
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

    printf("Translation Server 2 (English → French) running on port %d...\n", PORT);

    while ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len)) != INVALID_SOCKET) {
        handle_translation(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
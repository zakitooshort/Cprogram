#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib") // Nécessaire pour utiliser Winsock

#define PORT 8081
#define BUFFER_SIZE 1024

int main()
{
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char selected_word[50];

    // Initialiser les paramètres régionaux pour gérer les caractères français
    setlocale(LC_ALL, "en_US.UTF-8");

    // Initialiser Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Créer un socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Configurer l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // Se connecter au serveur
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        perror("Connection failed");
        return 1;
    }

    // Recevoir la liste des mots
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);

    // Saisir le mot choisi
    printf("Your choice: ");
    scanf("%s", selected_word);
    send(client_socket, selected_word, strlen(selected_word), 0);

    // Recevoir la traduction
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);

    // Fermer le socket
    closesocket(client_socket);
    WSACleanup();
    return 0;
}

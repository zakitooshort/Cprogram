#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib") // Nécessaire pour Winsock

#define PORT 8082
#define BUFFER_SIZE 1024

// Dictionnaire des traductions Anglais → Arabe
typedef struct
{
    char english[50];
    char arabic[50];
} Translation;

Translation dictionary[] = {
    {"dog", "كلب"},
    {"cat", "قط"},
    {"house", "منزل"}};
int dictionary_size = 3;

void handle_client(SOCKET client_socket)
{
    char buffer[BUFFER_SIZE] = {0};
    char translation[50] = "Mot non trouvé.";

    // Recevoir le mot à traduire
    recv(client_socket, buffer, BUFFER_SIZE, 0);

    // Rechercher la traduction dans le dictionnaire
    for (int i = 0; i < dictionary_size; i++)
    {
        if (strcmp(buffer, dictionary[i].english) == 0)
        {
            strcpy(translation, dictionary[i].arabic);
            break;
        }
    }

    // Envoyer la traduction au client
    send(client_socket, translation, strlen(translation), 0);
}

int main()
{
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size = sizeof(client_addr);

    // Initialiser Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Créer le socket serveur
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        perror("Erreur de création du socket");
        return 1;
    }

    // Configurer l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Lier le socket à l'adresse
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        perror("Erreur de liaison du socket");
        return 1;
    }

    // Écouter les connexions
    listen(server_socket, 3);
    printf("Serveur Anglais → Arabe en attente de connexions...\n");

    // Accepter les connexions
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size)) != INVALID_SOCKET)
    {
        printf("Client connecté.\n");
        handle_client(client_socket);
        closesocket(client_socket);
    }

    // Fermer le socket serveur
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

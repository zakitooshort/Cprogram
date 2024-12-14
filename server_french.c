#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib") // Nécessaire pour utiliser Winsock

#define PORT 8081
#define BUFFER_SIZE 1024

// Structure pour stocker les mots et leurs traductions
typedef struct
{
    char english[50];
    char french[50];
} Translation;

Translation translations[] = {
    {"dog", "chien"},
    {"cat", "chat"},
    {"house", "maison"},
    {"book", "livre"},
    {"car", "voiture"}};

void handle_client(SOCKET client_socket)
{
    char buffer[BUFFER_SIZE];
    char selected_word[50];
    int found = 0;

    // Étape 1 : Envoyer la liste des mots en anglais au client
    strcpy(buffer, "Choose a word to translate: ");
    for (int i = 0; i < sizeof(translations) / sizeof(translations[0]); i++)
    {
        strcat(buffer, translations[i].english);
        strcat(buffer, " ");
    }
    strcat(buffer, "\n");
    send(client_socket, buffer, strlen(buffer), 0);

    // Étape 2 : Recevoir le mot choisi par le client
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    sscanf(buffer, "%s", selected_word);

    // Rechercher le mot sélectionné
    for (int i = 0; i < sizeof(translations) / sizeof(translations[0]); i++)
    {
        if (strcmp(translations[i].english, selected_word) == 0)
        {
            snprintf(buffer, BUFFER_SIZE, "The translation in French is: %s\n", translations[i].french);
            found = 1;
            break;
        }
    }

    // Si le mot n'est pas trouvé
    if (!found)
    {
        strcpy(buffer, "Word not found. Please try again.\n");
    }

    // Envoyer la réponse au client
    send(client_socket, buffer, strlen(buffer), 0);
}

int main()
{
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size = sizeof(client_addr);

    // Initialiser les paramètres régionaux pour gérer les caractères français
    setlocale(LC_ALL, "en_US.UTF-8");

    // Initialiser Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Créer un socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Configurer le serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Lier le socket à l'adresse et au port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        perror("Bind failed");
        return 1;
    }

    // Écouter les connexions entrantes
    listen(server_socket, 3);
    printf("Server is running and waiting for connections...\n");

    // Gérer les connexions des clients
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size)) != INVALID_SOCKET)
    {
        printf("Client connected.\n");
        handle_client(client_socket);
        closesocket(client_socket);
    }

    // Fermer le socket serveur
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Nécessaire pour Winsock

#define PORT 8080
#define BUFFER_SIZE 1024

// Liste des mots disponibles
char *words[] = {"dog", "cat", "house"};
int word_count = 3;

// Fonction pour connecter à un serveur de traduction
SOCKET connect_to_translation_server(const char *ip, int port)
{
    SOCKET sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        perror("Erreur de création du socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        perror("Erreur de connexion au serveur de traduction");
        exit(1);
    }

    return sock;
}

// Fonction pour traiter un client
void handle_client(SOCKET client_socket)
{
    char buffer[BUFFER_SIZE] = {0};
    int selected_word_index = -1;
    char selected_language[BUFFER_SIZE];

    // Étape 1 : Envoyer la liste des mots au client
    strcpy(buffer, "Mots disponibles : ");
    for (int i = 0; i < word_count; i++)
    {
        strcat(buffer, words[i]);
        if (i < word_count - 1)
            strcat(buffer, ", ");
    }
    strcat(buffer, "\nChoisissez un mot : ");
    send(client_socket, buffer, strlen(buffer), 0);

    // Étape 2 : Recevoir le mot choisi par le client
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    buffer[strcspn(buffer, "\n")] = '\0'; // Retirer le saut de ligne

    // Vérifier si le mot existe
    for (int i = 0; i < word_count; i++)
    {
        if (strcmp(buffer, words[i]) == 0)
        {
            selected_word_index = i;
            break;
        }
    }

    if (selected_word_index == -1)
    {
        strcpy(buffer, "Mot non trouvé.\n");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    // Étape 3 : Demander la langue cible
    strcpy(buffer, "Choisissez une langue (french/arabe) : ");
    send(client_socket, buffer, strlen(buffer), 0);

    // Étape 4 : Recevoir la langue choisie
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    buffer[strcspn(buffer, "\n")] = '\0'; // Retirer le saut de ligne
    strcpy(selected_language, buffer);

    // Étape 5 : Connecter au serveur de traduction approprié
    SOCKET translation_socket;
    if (strcmp(selected_language, "french") == 0)
    {
        translation_socket = connect_to_translation_server("127.0.0.1", 8081); // Serveur français
    }
    else if (strcmp(selected_language, "arabe") == 0)
    {
        translation_socket = connect_to_translation_server("127.0.0.1", 8082); // Serveur arabe
    }
    else
    {
        strcpy(buffer, "Langue non reconnue.\n");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    // Envoyer le mot au serveur de traduction
    strcpy(buffer, words[selected_word_index]);
    send(translation_socket, buffer, strlen(buffer), 0);

    // Recevoir la traduction
    memset(buffer, 0, BUFFER_SIZE);
    recv(translation_socket, buffer, BUFFER_SIZE, 0);

    // Envoyer la traduction au client
    send(client_socket, buffer, strlen(buffer), 0);

    // Fermer la connexion au serveur de traduction
    closesocket(translation_socket);
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
    printf("Serveur de gestion en attente de connexions...\n");

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

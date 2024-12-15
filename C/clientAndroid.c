#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 5000
#define BUFFER_SIZE 1024

void handle_client() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char word[BUFFER_SIZE], language[BUFFER_SIZE];

    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    
    if (inet_pton(AF_INET, "192.168.1.100", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter one of these (hello,goodmorning,goodnight,cat,dog) (or 'exit' to quit) (hello,world,computer,programming,language): \n");
        fgets(word, BUFFER_SIZE, stdin);
        word[strcspn(word, "\n")] = 0; 
        if (strcmp(word, "exit") == 0) {
            break;
        }

        printf("Enter target language (AR/FR): ");
        fgets(language, BUFFER_SIZE, stdin);
        language[strcspn(language, "\n")] = 0;

        snprintf(buffer, BUFFER_SIZE, "%s|%s", word, language);

        
        send(sock, buffer, strlen(buffer), 0);

        
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, BUFFER_SIZE, 0);
        printf("Response: %s\n", buffer);
    }

    close(sock);
}

int main() {
    handle_client();
    return 0;
}
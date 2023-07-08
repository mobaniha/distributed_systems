#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    int clientSocket;
    struct sockaddr_in serverAddress;

    // Create client socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address properties
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(9000);
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
        perror("Invalid server address");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    char message[1024];
    printf("Enter a message to send: ");
    fgets(message, sizeof(message), stdin);

    // Send the message to the server
    if (send(clientSocket, message, strlen(message), 0) == -1) {
        perror("Message sending failed");
        exit(EXIT_FAILURE);
    }

    printf("Message sent successfully.\n");

    close(clientSocket);
    return 0;
}

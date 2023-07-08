#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_QUEUE_SIZE 100

typedef struct {
    int data[MAX_QUEUE_SIZE];
    int front;
    int rear;
} Queue;

Queue queue;

void initializeQueue() {
    queue.front = -1;
    queue.rear = -1;
}

int isQueueEmpty() {
    return (queue.front == -1 && queue.rear == -1);
}

int isQueueFull() {
    return ((queue.rear + 1) % MAX_QUEUE_SIZE == queue.front);
}

void enqueue(int value) {
    if (isQueueFull()) {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }

    if (isQueueEmpty()) {
        queue.front = 0;
        queue.rear = 0;
    } else {
        queue.rear = (queue.rear + 1) % MAX_QUEUE_SIZE;
    }

    queue.data[queue.rear] = value;
}

int dequeue() {
    if (isQueueEmpty()) {
        printf("Queue is empty. Cannot dequeue.\n");
        return -1;
    }

    int value = queue.data[queue.front];

    if (queue.front == queue.rear) {
        queue.front = -1;
        queue.rear = -1;
    } else {
        queue.front = (queue.front + 1) % MAX_QUEUE_SIZE;
    }

    return value;
}

void* handleClient(void* arg) {
    int clientSocket = *((int*)arg);

    while (1) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // Receive data from client
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            // Client disconnected or error occurred
            break;
        }

        // Convert received data to an integer
        int value = atoi(buffer);

        // Put the value onto the queue
        enqueue(value);

        printf("Received value: %d\n", value);
    }

    close(clientSocket);
    pthread_exit(NULL);
}

int main() {
    initializeQueue();

    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address properties
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(9000);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind server socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Socket listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Listening on port 9000...\n");

    while (1) {
        // Accept a client connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength)) == -1) {
            perror("Client connection acceptance failed");
            exit(EXIT_FAILURE);
        }

        printf("Client connected. Handling request...\n");

        // Create a new thread to handle the client
        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, (void*)&clientSocket) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        // Detach the thread (no need to join)
        pthread_detach(thread);
    }

    close(serverSocket);
    return 0;
}

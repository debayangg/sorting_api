#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

// Comparator function for qsort
int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void handle_request(int client_socket) {
    char buffer[1024] = {0};
    char response[1024] = {0};
    char sorted_response[1024] = {0};
    int array[100];
    int array_size = 0;
    int n;
    int expected_length = 0;

    // Read the HTTP request
    n = read(client_socket, buffer, sizeof(buffer));
    if (n <= 0) {
        perror("Error reading from socket");
        return;
    }

    // Find the array length within square brackets
    char* array_start = strchr(buffer, '[');
    if (!array_start) {
        strcpy(response, "HTTP/1.1 400 Bad Request\r\nContent-Length: 20\r\n\r\nMissing array start");
        write(client_socket, response, strlen(response));
        return;
    }

    array_start++;
    char* array_end = strchr(array_start, ']');
    if (!array_end) {
        strcpy(response, "HTTP/1.1 400 Bad Request\r\nContent-Length: 20\r\n\r\nMissing array end");
        write(client_socket, response, strlen(response));
        return;
    }

    *array_end = '\0';
    expected_length = atoi(array_start);

    // Parse the array from within square brackets
    char* array_data = array_end + 1;
    char* token = strtok(array_data, ",");
    while (token != NULL && array_size < expected_length && array_size < 100) {
        array[array_size++] = atoi(token);
        token = strtok(NULL, ",");
    }

    // Check if expected length matches actual array size
    if (array_size != expected_length) {
        strcpy(response, "HTTP/1.1 400 Bad Request\r\nContent-Length: 22\r\n\r\nArray size does not match expected length");
        write(client_socket, response, strlen(response));
        return;
    }

    // Sort the array using qsort
    qsort(array, array_size, sizeof(int), compare);

    // Prepare the response
    strcpy(sorted_response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
    for (int i = 0; i < array_size; ++i) {
        char num_str[10];
        sprintf(num_str, "%d", array[i]);
        strcat(sorted_response, num_str);
        if (i != array_size - 1) {
            strcat(sorted_response, ",");
        }
    }

    // Send the sorted array in the response
    write(client_socket, sorted_response, strlen(sorted_response));
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections
    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Handle HTTP request
        handle_request(client_socket);

        // Close the client socket
        close(client_socket);
    }

    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>

// Simple HTTP Server (for demonstration purposes)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Function to sort JSON array input
std::string sortJsonArray(const std::vector<int>& array) {
    // Sort the array
    std::vector<int> sortedArray = array;
    std::sort(sortedArray.begin(), sortedArray.end());

    // Convert sorted array to JSON string
    std::stringstream sortedJson;
    sortedJson << "[";
    for (size_t i = 0; i < sortedArray.size(); ++i) {
        if (i > 0) sortedJson << ",";
        sortedJson << sortedArray[i];
    }
    sortedJson << "]";

    return sortedJson.str();
}

// Function to handle incoming connections
void handleClient(int clientSocket) {
    char buffer[1024] = {0};
    std::string response;

    // Read request from client
    int valread = read(clientSocket, buffer, 1024);
    if (valread > 0) {
        // Parse JSON array from request
        std::string requestBody(buffer);
        std::cout << requestBody<< std::endl;
        std::vector<int> array;
        size_t pos = requestBody.find("[");
        size_t endpos = requestBody.find("]");
        if (pos != std::string::npos && endpos != std::string::npos) {
            std::string jsonArray = requestBody.substr(pos + 1, endpos - pos - 1);
            std::istringstream iss(jsonArray);
            std::string num;
            while (std::getline(iss, num, ',')) {
                try {
                    array.push_back(std::stoi(num));
                } catch (const std::invalid_argument& ia) {
                    std::cerr << "Invalid number: " << num << std::endl;
                }
            }
        }

        // Sort the array
        std::string sortedArray = sortJsonArray(array);

        // Prepare response
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: application/json\r\n";
        response += "Content-Length: " + std::to_string(sortedArray.size()) + "\r\n\r\n";
        response += sortedArray;
    } else {
        response = "HTTP/1.1 400 Bad Request\r\n\r\n";
    }

    // Send response to client
    send(clientSocket, response.c_str(), response.size(), 0);
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    // Prepare server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind server address to socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    // Start listening for incoming connections
    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    // Accept incoming connections and handle requests
    while (true) {
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        // Handle client request in a separate thread or process
        handleClient(clientSocket);
    }

    return 0;
}

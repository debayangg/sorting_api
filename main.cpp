#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>

using namespace boost::asio;
using ip::tcp;

// Function to sort JSON array input
std::string sortJsonArray(const std::vector<int>& array) {
    // Sort the array
    std::vector<int> sortedArray = array;
    std::sort(sortedArray.begin(), sortedArray.end());

    // Convert sorted array to JSON string
    nlohmann::json sortedJson = sortedArray;
    return sortedJson.dump();
}

// Function to handle incoming connections
void handleClient(tcp::socket socket) {
    try {
        boost::system::error_code error;

        // Read request headers
        boost::asio::streambuf buffer;
        boost::asio::read_until(socket, buffer, "\r\n\r\n", error);

        if (!error) {
            // Extract headers from buffer
            std::istream requestStream(&buffer);
            std::string headerLine;
            std::vector<std::string> headers;
            while (std::getline(requestStream, headerLine) && headerLine != "\r") {
                headers.push_back(headerLine);
            }

            // Find the input header
            std::string inputHeader;
            for (const std::string& header : headers) {
                if (boost::starts_with(header, "input:")) {
                    inputHeader = header.substr(6); // Skip "input:"
                    break;
                }
            }

            // Parse JSON array from input header
            if (!inputHeader.empty()) {
                boost::algorithm::trim(inputHeader);
                nlohmann::json json = nlohmann::json::parse(inputHeader);
                std::vector<int> array = json.get<std::vector<int>>();

                // Sort the array
                std::string sortedArray = sortJsonArray(array);

                // Prepare response
                std::ostringstream response;
                response << "HTTP/1.1 200 OK\r\n";
                response << "Content-Type: application/json\r\n";
                response << "Content-Length: " << sortedArray.size() << "\r\n\r\n";
                response << sortedArray;

                // Send response to client
                boost::asio::write(socket, boost::asio::buffer(response.str()), error);
            } else {
                std::string response = "HTTP/1.1 400 Bad Request\r\n\r\n";
                boost::asio::write(socket, boost::asio::buffer(response), error);
            }
        } else {
            std::string response = "HTTP/1.1 400 Bad Request\r\n\r\n";
            boost::asio::write(socket, boost::asio::buffer(response), error);
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    socket.close();
}

int main() {
    try {
        boost::asio::io_context io_context;

        // Create an acceptor to listen for incoming connections
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        std::cout << "Server listening on port 8080..." << std::endl;

        while (true) {
            // Create a socket and wait for a connection
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            // Handle the connection in a separate thread or process
            std::thread(handleClient, std::move(socket)).detach();
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

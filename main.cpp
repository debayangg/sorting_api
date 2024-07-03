#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <boost/asio.hpp>
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
            // Log headers
            std::istream requestStream(&buffer);
            std::string header;
            std::cout << "Request Headers:" << std::endl;
            while (std::getline(requestStream, header) && header != "\r") {
                std::cout << header << std::endl;
            }

            // Extract the content length
            std::string requestBody;
            size_t contentLength = 0;
            while (std::getline(requestStream, header) && header != "\r") {
                if (header.find("Content-Length:") != std::string::npos) {
                    contentLength = std::stoul(header.substr(16));
                }
            }

            // Read request body
            if (contentLength > 0) {
                std::vector<char> body(contentLength);
                requestStream.read(body.data(), contentLength);
                requestBody.assign(body.begin(), body.end());
                std::cout << "Request Body:" << std::endl << requestBody << std::endl;
            }

            // Parse JSON array from request body
            size_t inputPos = requestBody.find("input=");
            if (inputPos != std::string::npos) {
                std::string jsonArray = requestBody.substr(inputPos + 6); // Skip "input="
                jsonArray = jsonArray.substr(1, jsonArray.size() - 2); // Remove enclosing quotes

                // Parse JSON array
                nlohmann::json json = nlohmann::json::parse(jsonArray);
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

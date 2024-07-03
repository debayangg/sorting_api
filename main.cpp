#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

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
void handleClient(boost::asio::ip::tcp::socket& socket) {
    boost::asio::streambuf buffer;
    boost::system::error_code error;

    // Read request from client
    boost::asio::read_until(socket, buffer, "\r\n\r\n", error);
    if (!error) {
        std::istream requestStream(&buffer);
        std::string requestBody;
        std::string line;
        while (std::getline(requestStream, line) && line != "\r") {
            requestBody += line + "\n";
        }

        // Find the form data
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
    socket.close();
}

int main() {
    try {
        boost::asio::io_context ioContext;
        boost::asio::ip::tcp::acceptor acceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));

        std::cout << "Server listening on port 8080..." << std::endl;

        while (true) {
            boost::asio::ip::tcp::socket socket(ioContext);
            acceptor.accept(socket);
            handleClient(socket);
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

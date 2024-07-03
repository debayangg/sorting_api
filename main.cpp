#include <crow.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

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

int main() {
    crow::SimpleApp app;

    // Define a route for handling POST requests to '/sort'
    CROW_ROUTE(app, "/sort")
    .methods("POST"_method)
    ([](const crow::request& req) {
        // Parse JSON array from request body
        std::vector<int> array;
        try {
            auto json = crow::json::load(req.body);
            if (!json) {
                throw std::invalid_argument("Invalid JSON");
            }
            for (auto& elem : json) {
                array.push_back(elem.i());
            }
        } catch (const std::exception& e) {
            return crow::response(400, "Invalid JSON format");
        }

        // Sort the array
        std::string sortedArray = sortJsonArray(array);

        // Return sorted array as JSON response
        return crow::response(200, sortedArray);
    });

    // Run the Crow server on port 8080
    app.port(8080).multithreaded().run();

    return 0;
}

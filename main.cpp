#include <iostream>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <crow.h>

using json = nlohmann::json;

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/sort").methods("POST"_method)
    ([](const crow::request& req) {
        auto body = json::parse(req.body);
        std::vector<int> array = body.get<std::vector<int>>();

        std::sort(array.begin(), array.end());

        json response = array;
        return crow::response(response.dump());
    });

    app.port(8080).multithreaded().run();
}

#include <microhttpd.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstring>

// Function to sort JSON array input
std::string sortJsonArray(const std::string& input) {
    // Parse JSON array from input
    std::vector<int> array;
    std::stringstream ss(input);
    char c;
    int num;
    while (ss >> c >> num) {
        array.push_back(num);
    }

    // Sort the array
    std::sort(array.begin(), array.end());

    // Convert sorted array back to JSON string
    std::stringstream sortedJson;
    sortedJson << "[";
    for (size_t i = 0; i < array.size(); ++i) {
        if (i > 0) sortedJson << ",";
        sortedJson << array[i];
    }
    sortedJson << "]";

    return sortedJson.str();
}

// Request handler function
int requestHandler(void* cls, struct MHD_Connection* connection,
                   const char* url, const char* method,
                   const char* version, const char* upload_data,
                   size_t* upload_data_size, void** con_cls) {
    // Only handle POST requests
    if (strcmp(method, "POST") != 0)
        return MHD_NO; // Method Not Allowed

    // Read request data
    char* buffer = nullptr;
    size_t size = 0;
    MHD_get_connection_values(connection, MHD_POSTDATA_KIND, [](void* cls, enum MHD_ValueKind kind, const char* key, const char* value) {
        std::string* s = (std::string*)cls;
        if (s && key && value)
            *s += value;
        return MHD_YES;
    }, &buffer);

    // Sort the JSON array
    std::string sortedArray = sortJsonArray(buffer);

    // Send response
    struct MHD_Response* response = MHD_create_response_from_buffer(sortedArray.size(), (void*)sortedArray.c_str(), MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

int main() {
    struct MHD_Daemon* daemon;
    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, 8080, nullptr, nullptr,
                              &requestHandler, nullptr, MHD_OPTION_END);
    if (nullptr == daemon)
        return 1;
    std::cout << "Server running on port 8080..." << std::endl;
    (void)getchar();
    MHD_stop_daemon(daemon);
    return 0;
}

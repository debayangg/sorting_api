#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

// Parse JSON array from string input
vector<int> parseJsonArray(const string& input) {
    vector<int> array;
    stringstream ss(input);
    char c;
    int num;
    while (ss >> c >> num) {
        array.push_back(num);
    }
    return array;
}

// Convert vector to JSON string
string toJsonString(const vector<int>& array) {
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < array.size(); ++i) {
        if (i > 0) ss << ",";
        ss << array[i];
    }
    ss << "]";
    return ss.str();
}

int main() {
    string input;
    getline(cin, input);

    // Parse input as JSON array
    vector<int> array = parseJsonArray(input);

    // Sort the array
    sort(array.begin(), array.end());

    // Output sorted array as JSON string
    cout << toJsonString(array) << endl;

    return 0;
}

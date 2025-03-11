//
// Created by joshuawalker on 3/11/25.
//

#include "nlohmann/json.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    nlohmann::json test_json;
    nlohmann::json test_json_array = nlohmann::json::array({});
    nlohmann::json  json_obj1;
    nlohmann::json  json_obj2;

    test_json.emplace("test", "This is a test");

    std::cout << test_json << std::endl;

    test_json["test"] = "Hello";

    std::cout << test_json << std::endl;

    test_json["array"] = nullptr;

    test_json["array"]["element1"] = 1;
    test_json["array"]["element2"] = "two";
    test_json["array"]["element3"] = 3.0;

    std::cout << test_json << std::endl;

    json_obj1["Test"] = "Testing 1";
    json_obj2["Test"] = "Testing 2";

    test_json_array[0] = json_obj1;
    test_json_array[1] = json_obj2;

    std::cout << test_json_array << std::endl;

    test_json["array"] = test_json_array;

    std::cout << test_json << std::endl;

    std::cout << test_json["array"] << std::endl;

    std::cout << test_json.contains("array") << std::endl;
    std::cout << test_json.contains("array1") << std::endl;

    nlohmann::json copy = test_json["array"];

    for (int i = 0; i < copy.size(); i++) {
        std::cout << copy[i]["Test"] << std::endl;
    }

    test_json["secrets"] = "I love trailer park Brittney Spears";

    if (test_json.contains("secrets")) {
        std::cout << "Found one secret: " << test_json["secrets"] << std::endl;
    }

    return 0;
}

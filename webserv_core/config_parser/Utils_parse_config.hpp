#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>

enum method {
    GET,
    POST,
    DELETE,
    PUT
};


struct Location_info {
public:
    bool autoindex;
    std::vector<method> methods;
    int ret_num;
    int body_size;
    Location_info() : autoindex(true), methods(), ret_num(-1), body_size(200000) {}
};

namespace utils {
    int str_to_num(const std::string& to_convert);

};


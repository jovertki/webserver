#pragma once

#include <ostream>
#include "Utils_parse_config.hpp"
#define NOT_ASSIGN -1

struct Location_info {
    int autoIndex;
    int bodySize;
    std::string root;
    std::string uploadPath;
    std::string index;
    std::vector<method> methods;
    std::map<int, std::string> errorPage;
    std::pair<int, std::string> redirection; // not yet getter
    Location_info();

    friend std::ostream &operator<<(std::ostream &os, const Location_info &info);
};

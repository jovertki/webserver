#pragma once

#include <ostream>
#include "Utils_parse_config.hpp"
#define NOT_ASSIGN -1

struct Location_info {
    int autoindex;
    int bodySize;
    std::string root;
    std::string uploadPath;
    std::string index;
    std::vector<method> methods;
    std::map<int, std::string> errorPage;
    std::map<int, std::string> returnNum;
    Location_info();

    friend std::ostream &operator<<(std::ostream &os, const Location_info &info);
};

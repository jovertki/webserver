#pragma once
#include "Utils_parse_config.hpp"

struct Location_info {
    bool autoindex;
    std::vector<method> methods;
    int ret_num;
    int body_size;
    Location_info();
};

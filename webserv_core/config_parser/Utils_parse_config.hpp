#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include "../resources/defines.hpp"
// enum method {
//     EMPTY,
//     GET,
//     POST,
//     DELETE,
//     PUT
// };

namespace utils {

    int str_to_num(const std::string& to_convert);
    std::vector<std::string> make_tokens(std::ifstream& input);
    int strhex_to_num(const std::string& to_convert);
};


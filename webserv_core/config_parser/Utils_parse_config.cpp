#include "Utils_parse_config.hpp"

namespace utils {
    int str_to_num(const std::string& to_convert) {
        long int res;
        char *end_c;

        res = strtol(to_convert.c_str(), &end_c, 10);
        if ((errno == ERANGE && res == LONG_MAX) || res > INT_MAX ||
            (errno == ERANGE && res == LONG_MIN) || res < 0
            || to_convert.empty() || *end_c != '\0') {
            std::cout << "Config str_to_num!" << std::endl; // delete
            exit(-1); // make smart
        }
        return (int)res;
    }
}
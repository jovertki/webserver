#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>

enum method {
    EMPTY,
    GET,
    POST,
    DELETE,
    PUT
};

namespace utils {
    class parseExeption : public std::exception {
        const std::string m_msg;
    public:
        parseExeption(const std::string &msg);

        ~parseExeption() throw();

        const char *what() const throw();
    };

    int str_to_num(const std::string& to_convert);
    std::vector<std::string> make_tokens(std::ifstream& input);

};


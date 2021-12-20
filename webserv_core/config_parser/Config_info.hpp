#pragma once
#include "Server_info.hpp"
#include <vector>
#include <iostream>
#include <fstream>

class Config_info {
private:
    std::vector<Server_info> servers;
//    std::vector<std::string> tokens;
public:
    Config_info(const char* arg);
    ~Config_info() {};
private:
    void find_servers(std::vector<std::string>& lines);
    void make_tokens(std::ifstream& input);
    std::vector<std::string> make_lines(std::ifstream& input);
    void delete_lines();
};

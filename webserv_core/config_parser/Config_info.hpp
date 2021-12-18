#pragma once
#include "Server_info.hpp"
#include <vector>
#include <iostream>
#include <fstream>

class Config_info {
private:
    std::vector<Server_info> servers;
    std::vector<std::string> tokens;
public:
    Config_info(const char* arg);
    ~Config_info() {};
private:
    void find_servers();
    void makeTokens(std::ifstream& input);
};

#pragma once
#include "ServerConfig.hpp"


class Config_info {
private:
    std::vector<ServerConfig> servers;
public:
    Config_info(const char* arg);
    ~Config_info() {};
private:
    void identyServerValues(std::vector<std::string>& tokens);

};

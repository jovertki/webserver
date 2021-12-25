#pragma once
#include "Server_info.hpp"


class Config_info {
private:
    std::vector<Server_info> servers;
public:
    Config_info(const char* arg);
    ~Config_info() {};
private:
    void find_servers(std::vector<std::string>& lines);
    std::vector<std::string> make_tokens(std::ifstream& input);
    std::vector<std::string> make_lines(std::ifstream& input);
    void delete_lines();
};

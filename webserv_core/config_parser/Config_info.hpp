#pragma once
#include "ServerConfig.hpp"


class Config_info {
private:
    std::vector<ServerConfig> servers;
public:
    Config_info(const char* arg);
    ~Config_info() {};

    std::vector<ServerConfig> get_servers()const;

    std::string findLocationByID(const int servId, std::string locName);
    bool checkMethod(int id, std::string locName, method methodForCheck);
    int getAutoindex(const int servId, std::string locName);
    int getServerID(std::string hostIP, int portVal, std::string serverName = "");
private:

    void checkServNames();
    void identyServerValues(std::vector<std::string>& tokens);
};

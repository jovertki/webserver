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
    const int getServerID(const std::string hostIP, const int portVal, const std::string serverName = "");
private:

    void checkServNames();
    void identyServerValues(std::vector<std::string>& tokens);
};

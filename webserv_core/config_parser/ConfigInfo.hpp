#pragma once
#include "ServerConfig.hpp"

#define NOT_FOUND -1


class ConfigInfo {
private:
    std::vector<ServerConfig> servers;
public:
    explicit ConfigInfo(const char* arg);
    ~ConfigInfo();

    std::vector<ServerConfig> getServers()const;
    std::string getLocationByID(const int servId, std::string locName);
    int checkHostPortDublicates(int i);
    bool checkMethod(int id, std::string locName, method methodForCheck);
    int getAutoIndex(int servId, std::string locName);
    int getBodySize(int servId, std::string locName);
    std::string getRoot(int servId, std::string locName);
    std::string getUploadPath(int servId, std::string locName);
    std::string getIndex(int servId, std::string locName);
    std::string getErrorPage(int servId, std::string locName, int errorNum);
    const int getServerID(std::string hostIP, int portVal, std::string serverName = "");
private:

    void _checkServNames();
    void _identyServerValues(std::vector<std::string>& tokens);
};

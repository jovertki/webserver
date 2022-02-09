#pragma once
#include "ServerConfig.hpp"
#include "../utils/utils.hpp"

#define NOT_FOUND -1


class ConfigInfo {
private:
    std::vector<ServerConfig> servers;
public:
    explicit ConfigInfo(const char* arg);
    ~ConfigInfo();

    std::vector<ServerConfig> getServers()const;
    std::string getLocationByID(const int &servId, std::string locName)const;
    int checkHostPortDublicates(int i);
    bool checkMethod(const int& servId, std::string locName, const method& methodForCheck) const;
    int getAutoIndex(const int& servId, std::string locName) const ;
    int getBodySize(const int& servId, std::string locName) const ;
    std::string getCGI(const int& servId, const std::string& extentionCgi ) const ;
    std::string getRootedUrl(const int& servId, std::string locName) const ;
    std::string getUploadPath(const int& servId, std::string locName) const ;
    std::string getIndex(const int& servId, std::string locName) const ;
    std::string getRedirect(const int& servId, std::string locName, int& errorNum) const ;
    std::string getErrorPage(const int& servId, std::string locName, const int& errorNum) const ;
    int getServerID(const std::string& hostIP, const int& portVal, std::string serverName = "");
private:

    void _checkServNames();
    void _identyServerValues(std::vector<std::string>& tokens);
};

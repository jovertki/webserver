#pragma once
#include "Location_info.hpp"

class ServerConfig {
private:
    int listen;
    std::string servName;
    std::string host;
    std::map<std::string, std::string> cgi;
    std::map<std::string, Location_info> locations;
public:
    explicit ServerConfig(std::vector<std::string> tokens);
    ServerConfig(const ServerConfig& other);
    ServerConfig& operator=(const ServerConfig& other);
    ~ServerConfig();

    const std::string &getHost() const;
    const int &getListen() const;
    const std::string &getServName() const;
    const std::map<std::string, std::string> &getCgi() const;
    const std::map<std::string, Location_info> &getLocations() const;

private:
    static void checkSlashes(std::string& toCheck) ;
    void copyLocatData(std::string locName);
    void fillLocFromDefault();
    void checkAndFindValues(std::vector<std::string>& tokens);
    void CheckDefaultParam();
    static int findAutoIndex(std::vector<std::string>::iterator& iter);
    static int findIntAndIterate(std::vector<std::string>::iterator& iter);
    static void findErrorPage(std::vector<std::string>::iterator& iter,
                                     std::map<int, std::string>& errorPage);
    void findMainValues(std::vector<std::string>::iterator iter,
                     std::vector<std::string>::iterator end);
    void findCgi(std::vector<std::string>::iterator& iter);
    void findLocation(std::vector<std::string>::iterator& iter,
                        std::vector<std::string>::iterator& end);
    static std::string findStringAndIterate(std::vector<std::string>::iterator& iter);
    static std::vector<method> findMethods(std::vector<std::string>::iterator& iter,
                                    std::vector<std::string>::iterator& end);
    static Location_info findLocationParameters(std::vector<std::string>::iterator& iter,
                                          std::vector<std::string>::iterator& end);
    static std::pair<int, std::string> findReturn(std::vector<std::string>::iterator& iter);
};
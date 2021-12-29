#pragma once
#include "Utils_parse_config.hpp"
#include "Location_info.hpp"

class ServerConfig {
private:
    int listen;
    std::string servName;
    std::string root;
    std::string host;
    std::map<std::string, std::string> cgi;
    std::map<std::string, Location_info> locations;
public:
    explicit ServerConfig(std::vector<std::string> tokens);
    ServerConfig(const ServerConfig& other);
    ServerConfig& operator=(const ServerConfig& other);
    ~ServerConfig();

    const int &getListen() const;

    const std::string &getServName() const;

    const std::string &getRoot() const;

    const std::map<std::string, std::string> &getCgi() const;

    const std::map<std::string, Location_info> &getLocations() const;

private:

    int findAutoindex(std::vector<std::string>::iterator& iter);

    int findIntAndIterate(std::vector<std::string>::iterator& iter, int iterPlus);

    void findErrorPage(std::vector<std::string>::iterator& iter,
                                     std::map<int, std::string>& errorPage);

    void findMainValues(std::vector<std::string>::iterator iter,
                     std::vector<std::string>::iterator end);

    void findCgi(std::vector<std::string>::iterator& iter,
                  std::vector<std::string>::iterator& end);

    void findLocation(std::vector<std::string>::iterator& iter,
                        std::vector<std::string>::iterator& end);

    std::string findStringAndIterate(std::vector<std::string>::iterator& iter,
                                     int iterPlus);

    std::vector<method> findMethods(std::vector<std::string>::iterator& iter,
                                    std::vector<std::string>::iterator& end);

    Location_info findLocationParameters(std::vector<std::string>::iterator& iter,
                                          std::vector<std::string>::iterator& end);

    std::map<int, std::string> findReturn(std::vector<std::string>::iterator& iter);

};
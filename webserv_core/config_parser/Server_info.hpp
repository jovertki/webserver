#pragma once
#include <vector>
#include <iostream>
#include <map>

class Server_info {
private:
    std::vector<int> listen;
    std::string name;
    std::string root;
    bool autoindex;
    std::vector<int> methods; // ?? enum
    std::map<int, std::string> error_page;
    std::map<std::string, std::string> cgi;
//    std::map<std::string, Location_info> locations; // make it
public:
    Server_info(std::vector<std::string>::iterator first,
                std::vector<std::string>::iterator last);
};
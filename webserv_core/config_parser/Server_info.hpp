#pragma once
#include "Utils_parse_config.hpp"

class Server_info {
private:
    std::vector<int> listen;
    std::vector<std::string> serv_name;
    std::string root;
    bool autoindex;
    std::vector<method> methods; // ?? enum
    std::map<int, std::string> error_page;
    int body_size;
    std::map<std::string, std::string> cgi;
    std::map<std::string, Location_info> locations; // make it
public:
    Server_info(std::vector<std::string> tokens);

private:
    void find_values(std::vector<std::string>& tokens);
    void find_listen(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end);
    void find_serv_name(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end);
    void find_root(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end);
    bool find_autoindex(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end);
    std::vector<method> find_methods(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end);
    void find_error_page(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end);
    int find_int(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end);
    void find_cgi(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end);
    void find_locations(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end);

};
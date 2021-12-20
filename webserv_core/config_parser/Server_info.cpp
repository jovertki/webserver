#include "Server_info.hpp"

Server_info::Server_info(std::vector<std::string> tokens) : listen(), name(), root(), autoindex(true),
                         methods(), error_page(), body_size(), cgi() {
    std::cout << "Server :" << std::endl; // delete
    for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
        std::cout << *iter << std::endl; // delete
    find_values(tokens);
}

void Server_info::find_values(std::vector<std::string>& tokens) {
    find_listen(tokens);
    find_name(tokens);
    find_root(tokens);
    find_autoindex(tokens);
    find_methods(tokens);
    find_error_page(tokens);
    find_cgi(tokens);
//    find_locations(tokens);
}

void Server_info::find_listen(std::vector<std::string>& tokens) {

}

void Server_info::find_name(std::vector<std::string>& tokens) {

}

void Server_info::find_root(std::vector<std::string>& tokens) {

}

void Server_info::find_autoindex(std::vector<std::string>& tokens) {

}

void Server_info::find_methods(std::vector<std::string>& tokens) {

}

void Server_info::find_error_page(std::vector<std::string>& tokens) {

}

void Server_info::find_cgi(std::vector<std::string>& tokens) {

}
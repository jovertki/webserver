#include "Server_info.hpp"

Server_info::Server_info(std::vector<std::string> tokens) : listen(), serv_name(), root(), autoindex(true),
                         methods(), error_page(), body_size(-1), cgi() {
    std::cout << "Server :" << std::endl; // delete
    find_values(tokens);
}

void Server_info::find_values(std::vector<std::string>& tokens) {
    std::vector<std::string>::iterator iter = tokens.begin() + 1, end = tokens.end();
    if (tokens.front() != "{" || tokens.back() != "}" || tokens.size() < 4) {
        std::cout << "Config error Server_info::find_values!" << std::endl; // delete
        exit(-1); // make smart
    }
    for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
        std::cout << *it << std::endl; // delete
    while (iter + 2 < end) {
        if (*iter == "listen" && *(iter + 2) == ";")
            find_listen(iter, end);
        else if (*iter == "server_name" && *(iter + 2) == ";")
            find_serv_name(iter, end);
        else if (*iter == "root" && *(iter + 2) == ";")
            find_root(iter, end);
        else if (*iter == "autoindex" && *(iter + 2) == ";")
            find_autoindex(iter, end);
        else if (*iter == "allow_methods" && *(iter + 2) == ";")
            find_methods(iter, end);
        else if (*iter == "error_page" && *(iter + 3) == ";")
            find_error_page(iter, end);
        else if (*iter == "body_size" && *(iter + 2) == ";")
            find_body_size(iter, end);
        else if (*iter == "cgi" && *(iter + 3) == ";")
            find_cgi(iter, end);
        else
            ++iter; // delete;
//        if (*iter == "location" && *(iter + 2) == "{")
//    find_locations(iter, end);
//        else
//            some error statement;

    }
    for (; iter != tokens.end(); ++iter)
        std::cout << *iter << std::endl; // delete
//    std::cout << "Config error Server_info::find_values !" << std::endl; // delete
//    exit(-1); // make smart
}

void Server_info::find_listen(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end) {
    ++iter;
    int res;
    char *end_c;
    std::string to_convert = *iter;
    res = strtol(to_convert.c_str(), &end_c, 10);
    if ((errno == ERANGE && res == LONG_MAX) || res > INT_MAX ||
        (errno == ERANGE && res == LONG_MIN) || res < 0
        || to_convert.empty() || *end_c != '\0') {
        std::cout << "Config error Server_info::find_listen !" << std::endl; // delete
        exit(-1); // make smart
    }
//    std::istringstream string_num(*iter);
//    string_num >> res;
//    if (string_num.fail()) {
//        std::cout << "Config error Server_info::find_listen !" << std::endl; // delete
//        exit(-1); // make smart
//    }
    listen.push_back(res);
    iter += 2;
    std::cout << "result listen = " << res << " iter = " << *iter  << std::endl; // delete
}

void Server_info::find_serv_name(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end) {
    ++iter;
    std::string to_convert = *iter;
    if (serv_name.size() || to_convert.empty()) {
        std::cout << "Config error Server_info::find_listen !" << std::endl; // delete
        exit(-1); // make smart
    }
    serv_name = to_convert;
    iter += 2;
    std::cout << "result serv_name = " << serv_name << " iter = " << *iter  << std::endl; // delete
}

void Server_info::find_body_size(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end) {
    ++iter;
    int res;
    char *end_c;
    std::string to_convert = *iter;
    res = strtol(to_convert.c_str(), &end_c, 10);
    if ((errno == ERANGE && res == LONG_MAX) || res > INT_MAX ||
        (errno == ERANGE && res == LONG_MIN)|| res < 0
        || to_convert.empty() || *end_c != '\0' || body_size != -1) {
        std::cout << "Config error Server_info::find_body_size !" << std::endl; // delete
        exit(-1); // make smart
    }
//    std::istringstream string_num(*iter);
//    string_num >> res;
//    if (string_num.fail()) {
//        std::cout << "Config error Server_info::find_listen !" << std::endl; // delete
//        exit(-1); // make smart
//    }
    body_size = res;
    iter += 2;
    std::cout << "result body_size = " << res << " iter = " << *iter  << std::endl; // delete
}

void Server_info::find_root(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end) {
    ++iter;
    std::string to_convert = *iter;
    if (root.size() || to_convert.empty()) {
        std::cout << "Config error Server_info::root !" << std::endl; // delete
        exit(-1); // make smart
    }
    root = to_convert;
    iter += 2;
    std::cout << "result root = " << root << " iter = " << *iter  << std::endl; // delete
}


void Server_info::find_autoindex(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end) {

    ++iter;
}

void Server_info::find_methods(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end) {

    ++iter;
}

void Server_info::find_error_page(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end) {

    ++iter;
}

void Server_info::find_cgi(std::vector<std::string>::iterator& iter, std::vector<std::string>::iterator& end) {

    ++iter;
}
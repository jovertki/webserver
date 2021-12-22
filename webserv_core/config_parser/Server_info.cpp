#include "Server_info.hpp"

Server_info::Server_info(std::vector<std::string> tokens) : listen(), serv_name(), root(), autoindex(true),
                         methods(), error_page(), body_size(2000), cgi() {
    std::cout << "Server :" << std::endl; // delete
    find_values(tokens);
}

void Server_info::find_values(std::vector<std::string>& tokens) {
    std::vector<std::string>::iterator iter, end;
    bool autoind_check, root_check, methods_check, size_check;

    iter = tokens.begin() + 1;
    end = tokens.end(); // make it const
    autoind_check = root_check = methods_check = size_check = true;
    if (tokens.front() != "{" || tokens.back() != "}" || tokens.size() < 4) {
        std::cout << "Config error Server_info::find_values!" << std::endl; // delete
        exit(-1); // make smart
    }
//    for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
//        std::cout << *it << std::endl; // delete
    while (iter + 3 < end) {
        if (*iter == "listen" && *(iter + 2) == ";")
            find_listen(++iter, end);
        else if (*iter == "server_name" && *(iter + 2) == ";")
            find_serv_name(++iter, end);
        else if (*iter == "root" && *(iter + 2) == ";" && root_check) {
            find_root(++iter, end);
            root_check = false;
        }
        else if (*iter == "autoindex" && *(iter + 2) == ";" && autoind_check) {
            find_autoindex(++iter, end);
            autoind_check = false;
        }
        else if (*iter == "allow_methods" && methods_check) {
            find_methods(++iter, end);
            methods_check = false;
        }
        else if (*iter == "error_page" && *(iter + 3) == ";")
            find_error_page(++iter, end);
        else if (*iter == "body_size" && *(iter + 2) == ";" && size_check) {
            find_body_size(++iter, end);
            size_check = false;
        }
        else if (*iter == "cgi" && *(iter + 3) == ";")
            find_cgi(++iter, end);
//      else if (*iter == "location" && *(iter + 2) == "{")
//    find_locations(++iter, end);
//      else {
//          std::cout << "Config error Server_info::find_values !" << std::endl; // delete
//          exit(-1); // make smart
//    }
    }
    if (*iter != "}") {
        std::cout << "Config error Server_info::find_values!" << std::endl; // delete
        exit(-1); // make smart
    }
    for (; iter != tokens.end(); ++iter)
        std::cout << *iter << std::endl; // delete
}

void Server_info::find_listen(std::vector<std::string>::iterator& iter,
                              std::vector<std::string>::iterator& end) {
    int res;

    res = utils::str_to_num(*iter);
    if (std::find(listen.begin(), listen.end(), res) != listen.end()) {
        std::cout << "Config error Server_info::find_listen !" << std::endl; // delete
        exit(-1); // make smart
    }
    listen.push_back(res);
    iter += 2;
    std::cout << "result listen = " << res << " iter = " << *iter  << std::endl; // delete
}

void Server_info::find_serv_name(std::vector<std::string>::iterator& iter,
                                 std::vector<std::string>::iterator& end) {
    std::string to_convert = *iter;
    if (to_convert.empty() || std::find(serv_name.begin(),serv_name.end(),
                                        to_convert) != serv_name.end()) {
        std::cout << "Config error Server_info::find_serv_name !" << std::endl; // delete
        exit(-1); // make smart
    }
    serv_name.push_back(to_convert);
    iter += 2;
    std::cout << "result serv_name = " << serv_name.back() <<
    " iter = " << *iter  << std::endl; // delete
}

void Server_info::find_body_size(std::vector<std::string>::iterator& iter,
                                 std::vector<std::string>::iterator& end) {
    int res;

    res = utils::str_to_num(*iter);
    body_size = res;
    iter += 2;
    std::cout << "result body_size = " << res << " iter = " << *iter  << std::endl; // delete
}

void Server_info::find_root(std::vector<std::string>::iterator& iter,
                            std::vector<std::string>::iterator& end) {
    std::string to_convert = *iter;
    if (root.size() || to_convert.empty()) {
        std::cout << "Config error Server_info::root !" << std::endl; // delete
        exit(-1); // make smart
    }
    root = to_convert;
    iter += 2;
    std::cout << "result root = " << root << " iter = " << *iter  << std::endl; // delete
}


void Server_info::find_autoindex(std::vector<std::string>::iterator& iter,
                                 std::vector<std::string>::iterator& end) {
    if (*iter == "on")
        autoindex = false;
    else if(*iter == "off")
        autoindex = true;
    else {
        std::cout << "Config error Server_info::autoindex !" << std::endl; // delete
        exit(-1); // make smart
    }
    iter += 2;
}

void Server_info::find_methods(std::vector<std::string>::iterator& iter,
                               std::vector<std::string>::iterator& end) {
    bool get_check, post_check, del_check, put_check;

    get_check = post_check = del_check = put_check = true;
    while (iter + 1 < end && *iter != ";") {
        if (*iter == "GET" && get_check) {
            methods.push_back(GET);
            get_check = false;
        }
        else if(*iter == "POST" && post_check) {
            methods.push_back(POST);
            post_check = false;
        }
        else if(*iter == "DELETE" && del_check) {
            methods.push_back(DELETE);
            del_check = false;
        }
        else if(*iter == "PUT" && put_check) {
            methods.push_back(PUT);
            put_check = false;
        }
        else {
            std::cout << "Config error Server_info::methods !" << std::endl; // delete
            exit(-1); // make smart
        }
        ++iter;
    }
    ++iter;
    for (int i = 0; methods.begin() + i != methods.end(); ++i)
        std::cout << "method find n " << i << std::endl;
}

void Server_info::find_error_page(std::vector<std::string>::iterator& iter,
                                  std::vector<std::string>::iterator& end) {
    int res;

    res = utils::str_to_num(*iter);
    if (error_page.find(res) != error_page.end()) {
        std::cout << "Config error Server_info::duplicate error_page!" << std::endl; // delete
        exit(-1); // make smart
    }
    ++iter;
    error_page[res] = *iter;
    iter += 2;
    std::cout << "error_page[res] " << error_page[res] << " iter = " << *iter << std::endl;
}

void Server_info::find_cgi(std::vector<std::string>::iterator& iter,
                           std::vector<std::string>::iterator& end) {
    std::string key = *iter;
    if (cgi.find(key) != cgi.end()) {
        std::cout << "Config error Server_info::duplicate cgi!" << std::endl; // delete
        exit(-1); // make smart
    }
    ++iter;
    cgi[key] = *iter;
    iter += 2;
    std::cout << "cgi[*iter] " << cgi[key] << " iter = " << *iter << std::endl;
}
#include "ServerConfig.hpp"

ServerConfig::ServerConfig(std::vector<std::string> tokens) : listen(), serv_name(), root(), autoindex(true),
                                                              methods(), error_page(), body_size(2000), cgi() {
    find_values(tokens);
}

ServerConfig::ServerConfig(const ServerConfig& other) : listen(other.listen), serv_name(other.serv_name),
                                                        root(other.root), autoindex(other.autoindex),
                                                        methods(other.methods), error_page(other.error_page),
                                                        body_size(other.body_size), cgi(other.cgi) {}

ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
    if (this != &other)
    {
        listen = other.listen;
        serv_name = other.serv_name;
        root = other.root;
        autoindex = other.autoindex;
        methods = other.methods;
        error_page = other.error_page;
        body_size = other.body_size;
        cgi = other.cgi;
        locations = other.locations;
    }
    return *this;
}

ServerConfig::~ServerConfig() {}

void ServerConfig::find_values(std::vector<std::string>& tokens) {
    std::vector<std::string>::iterator iter, end;
    bool autoind_check, root_check, methods_check, size_check;

    iter = tokens.begin() + 1;
    end = tokens.end();
    autoind_check = root_check = methods_check = size_check = true;
    if (tokens.front() != "{" || tokens.back() != "}" || tokens.size() < 4)
        throw utils::MyException("Error ServerParse::find_values!");
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
            autoindex = find_autoindex(++iter, end);
            autoind_check = false;
        }
        else if (*iter == "allow_methods" && methods_check) {
            methods = find_methods(++iter, end);
            methods_check = false;
        }
        else if (*iter == "error_page" && *(iter + 3) == ";")
            find_error_page(++iter, end);
        else if (*iter == "body_size" && *(iter + 2) == ";" && size_check) {
            body_size = find_int(++iter, end);
            size_check = false;
        }
        else if (*iter == "cgi" && *(iter + 3) == ";")
            find_cgi(++iter, end);
        else if (*iter == "location" && *(iter + 2) == "{")
            find_locations(++iter, end);
        else
          throw utils::MyException("Error ServerParse::find_values!");
    }
    if (*iter != "}")
        throw utils::MyException("Error ServerParse::find_values!");
}

void ServerConfig::find_listen(std::vector<std::string>::iterator& iter,
                               std::vector<std::string>::iterator& end) {
    int res;

    res = utils::str_to_num(*iter);
    if (std::find(listen.begin(), listen.end(), res) != listen.end())
        throw utils::MyException("Error ServerParse::find_listen!");
    listen.push_back(res);
    iter += 2;
    std::cout << "result listen = " << res << std::endl; // delete
}

void ServerConfig::find_serv_name(std::vector<std::string>::iterator& iter,
                                  std::vector<std::string>::iterator& end) {
    std::string to_convert = *iter;
    if (to_convert.empty() || std::find(serv_name.begin(),serv_name.end(),
                                        to_convert) != serv_name.end())
        throw utils::MyException("Error ServerParse::find_serv_name!");
    serv_name.push_back(to_convert);
    iter += 2;
    std::cout << "result serv_name = " << serv_name.back() << std::endl; // delete
}

int ServerConfig::find_int(std::vector<std::string>::iterator& iter,
                           std::vector<std::string>::iterator& end) {
    int res;

    res = utils::str_to_num(*iter);
    iter += 2;
    std::cout << "result body_size = " << res << std::endl; // delete
    return res;
}

void ServerConfig::find_root(std::vector<std::string>::iterator& iter,
                             std::vector<std::string>::iterator& end) {
    std::string to_convert = *iter;
    if (root.size() || to_convert.empty())
        throw utils::MyException("Error ServerParse::find_root!");
    root = to_convert;
    iter += 2;
    std::cout << "result root = " << root << std::endl; // delete
}


bool ServerConfig::find_autoindex(std::vector<std::string>::iterator& iter,
                                  std::vector<std::string>::iterator& end) {
    bool result;
    if (*iter == "on")
        result = false;
    else if(*iter == "off")
        result = true;
    else
        throw utils::MyException("ServerParse::find_autoindex!");
    iter += 2;
    return result;
}

std::vector<method> ServerConfig::find_methods(std::vector<std::string>::iterator& iter,
                                               std::vector<std::string>::iterator& end) {
    bool get_check, post_check, del_check, put_check;
    std::vector<method> result;

    get_check = post_check = del_check = put_check = true;
    while (iter + 1 < end && *iter != ";") {
        if (*iter == "GET" && get_check) {
            result.push_back(GET);
            get_check = false;
        }
        else if(*iter == "POST" && post_check) {
            result.push_back(POST);
            post_check = false;
        }
        else if(*iter == "DELETE" && del_check) {
            result.push_back(DELETE);
            del_check = false;
        }
        else if(*iter == "PUT" && put_check) {
            result.push_back(PUT);
            put_check = false;
        }
        else
            throw utils::MyException("ServerParse::find_methods!");
        ++iter;
    }
    ++iter;
    return result;
}

void ServerConfig::find_error_page(std::vector<std::string>::iterator& iter,
                                   std::vector<std::string>::iterator& end) {
    int res;

    res = utils::str_to_num(*iter);
    if (error_page.find(res) != error_page.end())
        throw utils::MyException("ServerParse::find_error_page");
    ++iter;
    error_page[res] = *iter;
    iter += 2;
    std::cout << "result error_page " << error_page[res] << std::endl;
}

void ServerConfig::find_cgi(std::vector<std::string>::iterator& iter,
                            std::vector<std::string>::iterator& end) {
    std::string key = *iter;
    if (cgi.find(key) != cgi.end())
        throw utils::MyException("ServerParse::duplicate CGI!");
    ++iter;
    cgi[key] = *iter;
    iter += 2;
    std::cout << "result cgi " << cgi[key] << std::endl;
}

void ServerConfig::find_locations(std::vector<std::string>::iterator& iter,
                                  std::vector<std::string>::iterator& end) {
    std::string locationName = *iter;
    bool autoind_check, methods_check, size_check;

    autoind_check = methods_check = size_check = true;
    if (locations.find(locationName) != locations.end() || *(iter + 1) != "{")
        throw utils::MyException("ServerParse::duplicate locations!");
    iter += 2;
    while (iter + 2 < end && *iter != "}") {
        if (*iter == "body_size" && *(iter + 2) == ";" && size_check) {
            locations[locationName].body_size = find_int(++iter, end);
            size_check = false;
        }
        else if (*iter == "allow_methods" && methods_check) {
            locations[locationName].methods = find_methods(++iter, end);
            methods_check = false;
        }
        else if (*iter == "autoindex" && *(iter + 2) == ";" && autoind_check) {
            locations[locationName].autoindex = find_autoindex(++iter, end);
            autoind_check = false;
        }
        else if (*iter == "return" && *(iter + 2) == ";" && locations[locationName].ret_num == -1)
            locations[locationName].ret_num = find_int(++iter, end);
        else
            throw utils::MyException("ServerParse::locations!");
    }
    if (*iter != "}")
        throw utils::MyException("ServerParse::locations!");
    iter++;
}

const std::vector<int> &ServerConfig::getListen() const {
    return listen;
}

const std::vector<std::string> &ServerConfig::getServName() const {
    return serv_name;
}

const std::string &ServerConfig::getRoot() const {
    return root;
}

bool ServerConfig::isAutoindex() const {
    return autoindex;
}

const std::vector<method> &ServerConfig::getMethods() const {
    return methods;
}

const std::map<int, std::string> &ServerConfig::getErrorPage() const {
    return error_page;
}

int ServerConfig::getBodySize() const {
    return body_size;
}

const std::map<std::string, std::string> &ServerConfig::getCgi() const {
    return cgi;
}

const std::map<std::string, Location_info> &ServerConfig::getLocations() const {
    return locations;
}




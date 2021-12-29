#include "ServerConfig.hpp"

ServerConfig::ServerConfig(std::vector<std::string> tokens) : listen(), servName(), root(),
                                                              host(), cgi(), locations() {
    if (tokens.front() != "{" || tokens.back() != "}" || tokens.size() < 4) // можно проверить на больше 4-ч значений
        throw utils::MyException("Error ServerParse::find_values!");
    findMainValues(tokens.begin() + 1, tokens.end());
}

ServerConfig::ServerConfig(const ServerConfig& other) : listen(other.listen), servName(other.servName),
                                                        root(other.root), host(other.host), cgi(other.cgi),
                                                        locations(other.locations) {}

ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
    if (this != &other)
    {
        listen = other.listen;
        servName = other.servName;
        root = other.root;
        host = other.host;
        cgi = other.cgi;
        locations = other.locations;
    }
    return *this;
}

ServerConfig::~ServerConfig() {}

void ServerConfig::findMainValues(std::vector<std::string>::iterator iter,
                               std::vector<std::string>::iterator end) {
    while (iter + 3 < end) {
        std::cout << *iter << std::endl; // delete
        if (*iter == "listen" && *(iter + 2) == ";" && !listen && *(iter + 1) != "0")
            listen = findIntAndIterate(++iter, 2);
        else if (*iter == "server_name" && *(iter + 2) == ";" && servName.empty())
            servName = findStringAndIterate(++iter, 2);
        else if (*iter == "root" && *(iter + 2) == ";" && root.empty())
            root = findStringAndIterate(++iter, 2);
        else if (*iter == "host" && *(iter + 2) == ";" && host.empty())
            host = findStringAndIterate(++iter, 2);
        else if (*iter == "cgi" && *(iter + 3) == ";")
            findCgi(++iter, end);
        else if (*iter == "location" && *(iter + 2) == "{")
            findLocation(++iter, end);
        else
          throw utils::MyException("Error ServerParse::find_values!");
    }
    if (*iter != "}" || iter + 1 != end)
        throw utils::MyException("Error ServerParse::find_values!"); // можно перенести в функцию чекера
//  проверить на наличие всех нужных параметров
}

std::string ServerConfig::findStringAndIterate(std::vector<std::string>::iterator& iter, int iterPlus) {
    std::string res;

    res = *iter;
    iter += iterPlus;
    return res;
}

int ServerConfig::findIntAndIterate(std::vector<std::string>::iterator& iter, int iterPlus) {
    int res;

    res = utils::str_to_num(*iter);
    iter += iterPlus;
    return res;
}

void ServerConfig::findCgi(std::vector<std::string>::iterator& iter,
                           std::vector<std::string>::iterator& end) {
    std::string key;

    key = *iter;
    if (cgi.find(key) != cgi.end())
        throw utils::MyException("ServerParse::duplicate CGI!");
    ++iter;
    cgi[key] = *iter;
    iter += 2;
    std::cout << "result cgi " << cgi[key] << std::endl;
}

void ServerConfig::findLocation(std::vector<std::string>::iterator& iter,
                                  std::vector<std::string>::iterator& end) {
    std::string locationName = *iter;

    if (locations.find(locationName) != locations.end() || *(iter + 1) != "{")
        throw utils::MyException("ServerParse::duplicate locations!");
    iter += 2;
    locations[locationName] = findLocationParameters(iter, end);
    std::cout << locations[locationName] << std::endl; // delete
    if (*iter != "}")
        throw utils::MyException("ServerParse::locations!");
    if (locationName[locationName.size()] == '/' && locations[locationName].index.size())
        throw utils::MyException("ServerParse::locations with name index have index page!");
    iter++;
}

Location_info ServerConfig::findLocationParameters(std::vector<std::string>::iterator& iter,
                                      std::vector<std::string>::iterator& end) {
    Location_info res;
    while (iter + 4 < end && *iter != "}") {
        if (*iter == "body_size" && *(iter + 2) == ";" && res.bodySize == NOT_ASSIGN)
            res.bodySize = findIntAndIterate(++iter, 2);
        else if (*iter == "allow_methods" && res.methods.empty())
            res.methods = findMethods(++iter, end);
        else if (*iter == "autoindex" && *(iter + 2) == ";" && res.autoindex == NOT_ASSIGN)
            res.autoindex = findAutoindex(++iter); // нужно сделать по умолчанию в папках и обязательно в основе
        else if (*iter == "return" && res.returnNum.empty())
            res.returnNum = findReturn(++iter);
        else if (*iter == "error_page" && *(iter + 3) == ";")
            findErrorPage(++iter, res.errorPage);
        else if (*iter == "index" && *(iter + 2) == ";" && res.index.empty())
            res.index = findStringAndIterate(++iter, 2);
        else if (*iter == "upload_path" && *(iter + 2) == ";" && res.uploadPath.empty())
            res.uploadPath = findStringAndIterate(++iter, 2);
        else
            throw utils::MyException("ServerParse::locations!");
    }
//    написать копи контруктор для локешн структуры ??? или работает?
    return res;
}

void ServerConfig::findErrorPage(std::vector<std::string>::iterator& iter,
                                                       std::map<int, std::string>& errorPage) {
    int resInt;

    resInt = utils::str_to_num(*iter);
    if (errorPage.find(resInt) != errorPage.end())
        throw utils::MyException("ServerParse::find_error_page");
    ++iter;
    errorPage[resInt] = *iter;
    iter += 2;
}

std::map<int, std::string> ServerConfig::findReturn(std::vector<std::string>::iterator& iter) {
    std::map<int, std::string> resMap;
    int resInt;

    resInt = utils::str_to_num(*iter);
    ++iter;
    if (*iter != ";") {
        resMap[resInt] = *iter;
        iter += 2;
    }
    else {
        resMap[resInt];
        ++iter;
    }
    return resMap;
}


int ServerConfig::findAutoindex(std::vector<std::string>::iterator& iter) {
    int result;

    if (*iter == "on")
        result = false;
    else if(*iter == "off")
        result = true;
    else
        throw utils::MyException("ServerParse::find_autoindex!");
    iter += 2;
    return result;
}

std::vector<method> ServerConfig::findMethods(std::vector<std::string>::iterator& iter,
                                               std::vector<std::string>::iterator& end) {
    bool get_check, post_check, del_check, put_check;
    std::vector<method> result;

    get_check = post_check = del_check = put_check = true;
    while (iter + 2 < end && *iter != ";") {
        if (*iter == "GET" && get_check) { // возможно проверить по ключу ?
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

const int &ServerConfig::getListen() const {
    return listen;
}

const std::string &ServerConfig::getServName() const {
    return servName;
}

const std::string &ServerConfig::getRoot() const {
    return root;
}

const std::map<std::string, std::string> &ServerConfig::getCgi() const {
    return cgi;
}

const std::map<std::string, Location_info> &ServerConfig::getLocations() const {
    return locations;
}




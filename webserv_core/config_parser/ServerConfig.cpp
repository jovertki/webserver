#include "ServerConfig.hpp"

ServerConfig::ServerConfig(std::vector<std::string> tokens) : listen(), servName(),
                                                              host(), cgi(), locations() {
    checkAndFindValues(tokens);
}

ServerConfig::ServerConfig(const ServerConfig& other) : listen(other.listen), servName(other.servName),
                                                        host(other.host), cgi(other.cgi),
                                                        locations(other.locations) {}

ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
    if (this != &other)
    {
        listen = other.listen;
        servName = other.servName;
        host = other.host;
        cgi = other.cgi;
        locations = other.locations;
    }
    return *this;
}

ServerConfig::~ServerConfig() {}

void ServerConfig::checkAndFindValues(std::vector<std::string>& tokens) {
    if (tokens.front() != "{" || tokens.back() != "}" || tokens.size() < 25)
        throw utils::parseExeption("Error ServerParse::find values!");
    findMainValues(tokens.begin() + 1, tokens.end());
    CheckDefaultParam();
    // for (std::map<std::string, Location_info>:: iterator it = locations.begin();
    // it != locations.end(); ++it) {
    //    if ((*it).first.back() != '/' && (*it).second.index.size())
    //        throw utils::parseExeption("Error ServerParse::location file have index!");
    // }
    fillLocFromDefault();
}


void ServerConfig::fillLocFromDefault() {
    std::map<std::string, Location_info>::iterator it, end;

    for (it = locations.begin(), end = locations.end(); it != end; ++it) {
        if (it->first != "/")
            copyLocatData(it->first);
    }
}

void ServerConfig::copyLocatData(std::string locName) {
    if (locations[locName].redirection.empty()) {
        if (locations[locName].autoIndex == NOT_ASSIGN)
            locations[locName].autoIndex = locations["/"].autoIndex;
        if (locations[locName].bodySize == NOT_ASSIGN)
            locations[locName].bodySize = locations["/"].bodySize;
        if (locations[locName].index.empty() && locName.back() == '/')
            locations[locName].index = locations["/"].index;
        if (locations[locName].methods.empty())
            locations[locName].methods = locations["/"].methods;
        locations[locName].errorPage.insert(locations["/"].errorPage.begin(),
                                            locations["/"].errorPage.end());
    }
//    std::cout << "location name: " << locName << ". Content: " << locations[locName] << std::endl; // delete
    
}

void ServerConfig::CheckDefaultParam() {
    std::string defLocation;

    defLocation = "/";
    if (listen == 0)
        throw utils::parseExeption("Error ServerParse::no listen parameter!");
    else if (locations[defLocation].root.empty())
        throw utils::parseExeption("Error ServerParse::location no root!");
    else if(host.empty())
        throw utils::parseExeption("Error ServerParse::no host!");
    else if (locations.empty())
        throw utils::parseExeption("ServerParse::can't find loctions!");
    else if (locations.find(defLocation) == locations.end())
        throw utils::parseExeption("ServerParse::can't find mandatory location / !");
    else if (locations[defLocation].autoIndex == NOT_ASSIGN)
        throw utils::parseExeption("ServerParse::location can't find autoindex!");
    else if (locations[defLocation].bodySize == NOT_ASSIGN)
        throw utils::parseExeption("ServerParse::location can't find bodysize!");
    else if (locations[defLocation].index.empty())
        throw utils::parseExeption("ServerParse::location can't find indexh!");
    else if (locations[defLocation].uploadPath.empty())
        throw utils::parseExeption("ServerParse::location can't find upload_path!");
    else if (locations[defLocation].methods.empty())
        throw utils::parseExeption("ServerParse::location can't find methods!");
    else if (locations[defLocation].root.empty())
        throw utils::parseExeption("ServerParse::location can't find methods!");
}

void ServerConfig::findMainValues(std::vector<std::string>::iterator iter,
                               std::vector<std::string>::iterator end) {
    while (iter + 3 < end) {
        if (*iter == "listen" && *(iter + 2) == ";" && !listen && *(iter + 1) != "0")
            listen = findIntAndIterate(++iter, 2);
        else if (*iter == "server_name" && *(iter + 2) == ";" && servName.empty())
            servName = findStringAndIterate(++iter, 2);
        else if (*iter == "host" && *(iter + 2) == ";" && host.empty())
            host = findStringAndIterate(++iter, 2);
        else if (*iter == "cgi" && *(iter + 3) == ";")
            findCgi(++iter, end);
        else if (*iter == "location" && *(iter + 2) == "{")
            findLocation(++iter, end);
        else
          throw utils::parseExeption("Error ServerParse::server main parameters!");
    }
    if (*iter != "}" || iter + 1 != end)
        throw utils::parseExeption("Error ServerParse::last server parameter!");
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
        throw utils::parseExeption("ServerParse::duplicate CGI!");
    ++iter;
    cgi[key] = *iter;
    iter += 2;
}

void ServerConfig::findLocation(std::vector<std::string>::iterator& iter,
                                  std::vector<std::string>::iterator& end) {
    std::string locationName = *iter;

    if (locations.find(locationName) != locations.end() || *(iter + 1) != "{")
        throw utils::parseExeption("ServerParse::duplicate locations!");
    iter += 2;
    locations[locationName] = findLocationParameters(iter, end);
//    std::cout << locations[locationName] << std::endl; // delete
    if (*iter != "}")
        throw utils::parseExeption("ServerParse::locations!");
    if (locationName[locationName.size()] == '/' && locations[locationName].index.size())
        throw utils::parseExeption("ServerParse::locations with name index have index page!");
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
        else if (*iter == "root" && *(iter + 2) == ";" && res.root.empty())
            res.root = findStringAndIterate(++iter, 2);
        else if (*iter == "autoindex" && *(iter + 2) == ";" && res.autoIndex == NOT_ASSIGN)
            res.autoIndex = findAutoIndex(++iter);
        else if (*iter == "return" && res.redirection.empty())
            res.redirection = findReturn(++iter);
        else if (*iter == "error_page" && *(iter + 3) == ";")
            findErrorPage(++iter, res.errorPage);
        else if (*iter == "index" && *(iter + 2) == ";" && res.index.empty())
            res.index = findStringAndIterate(++iter, 2);
        else if (*iter == "upload_path" && *(iter + 2) == ";" && res.uploadPath.empty())
            res.uploadPath = findStringAndIterate(++iter, 2);
        else
            throw utils::parseExeption("ServerParse::locations!");
    }
    return res;
}

void ServerConfig::findErrorPage(std::vector<std::string>::iterator& iter,
                                                       std::map<int, std::string>& errorPage) {
    int resInt;

    resInt = utils::str_to_num(*iter);
    if (errorPage.find(resInt) != errorPage.end())
        throw utils::parseExeption("ServerParse::find_error_page");
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


int ServerConfig::findAutoIndex(std::vector<std::string>::iterator& iter) {
    int result;

    if (*iter == "on")
        result = 1;
    else if(*iter == "off")
        result = 0;
    else
        throw utils::parseExeption("ServerParse::find_autoindex!");
    iter += 2;
    return result;
}

std::vector<method> ServerConfig::findMethods(std::vector<std::string>::iterator& iter,
                                               std::vector<std::string>::iterator& end) {
    bool get_check, post_check, del_check, put_check;
    std::vector<method> result;

    get_check = post_check = del_check = put_check = true;
    while (iter + 2 < end && *iter != ";") {
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
            throw utils::parseExeption("ServerParse::find_methods!");
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

const std::map<std::string, std::string> &ServerConfig::getCgi() const {
    return cgi;
}

const std::map<std::string, Location_info> &ServerConfig::getLocations() const {
    return locations;
}

const std::string &ServerConfig::getHost() const {
    return host;
}




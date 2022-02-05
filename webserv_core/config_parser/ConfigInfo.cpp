#include "ConfigInfo.hpp"

ConfigInfo::ConfigInfo(const char* arg) : servers() {
    std::ifstream input(arg);
    try {
        std::vector<std::string> tokens = utils::make_tokens(input);
        _identyServerValues(tokens);
        _checkServNames();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
    //delete next line
    std::cout << "..........\n" << servers.size() << " servers successfully parsed" << std::endl;
//    std::cout << "ErrorPage 2222 on server 1(second) = " << getErrorPage(0, "/", 22) << std::endl;
//    std::cout << "ServID = " << getServerID("127.0.0.1", 8080, "cwd.localhost") << std::endl;
    // std::cout << "Check method = " << checkMethod(4, "/", DELETE) << std::endl;
    // exit(1);
}

ConfigInfo::~ConfigInfo() {}

/* Getters and checkers section */

/* -1 == NOT_FOUND. Checks only servers with ID less than i*/
int ConfigInfo::checkHostPortDublicates(int i) {
    int listenCheck;
    std::string hostCheck;

    listenCheck = servers[i].getListen();
    hostCheck = servers[i].getHost();
    while (i) {
        --i;
        if (servers[i].getListen() == listenCheck && hostCheck == servers[i].getHost())
            return i;
    }
    return NOT_FOUND;
}

std::string ConfigInfo::getCGI(const int& servId, const std::string& extentionCgi ) const {
    if (servers[servId].getCgi().find(extentionCgi) != servers[servId].getCgi().end())
        return servers[servId].getCgi().find(extentionCgi)->second;
    else
        return "";
}

/* Any locName will be truncated with "/" */
std::string ConfigInfo::getLocationByID(const int &servId, std::string locName) const {
    int pos;

    if (servId > servers.size() || locName.front() != '/') {
        std::cout << BOLDRED << "ERROR !!!!!!!\n\n\n\n ERROR !!!!!!!"
                                " \n getLocationByID\n\n\nERROR !!!!!!!" << RESET << std::endl;
    }
    while (locName.size() > 1) {
        if (servers[servId].getLocations().find(locName) != servers[servId].getLocations().end()) {
            return locName;
        }
        else {
            locName.pop_back();
            if ((pos = locName.find_last_of('/')) != std::string::npos) {
                if (pos == 0)
                    locName.resize(1);
                else
                    locName.resize(pos);
            }
        }
    }
    return locName;
}

bool ConfigInfo::checkMethod(const int& servId, std::string locName, const method& methodForCheck) const {
    std::vector<method> tempMethod;

    locName = getLocationByID(servId, locName);
    tempMethod = servers[servId].getLocations().find(locName)->second.methods;
    for (int i = 0; i + tempMethod.begin() < tempMethod.end(); ++i) {
        if (*(i + tempMethod.begin()) == methodForCheck)
            return true;
    }
    return false;
}

/* Вернет -1 если сервера с данными параметрами не найдено
 * Вернет ID первого в массиве сервера с подходящими параметрами, если данные совпадают */
int ConfigInfo::getServerID(const std::string& hostIP, const int& portVal, std::string serverName) {
    std::vector<ServerConfig>::iterator it, end;
    int res;
    if(serverName == hostIP)
        serverName = "";
    it = servers.begin();
    end = servers.end();
    res = -1;
    for (int i = 0; i + it < end; ++i) {
        if (hostIP == (it + i)->getHost()) {
            if (portVal == (it + i)->getListen()) {
                if (res == -1 && serverName.empty())
                    res = i;
                if (serverName.size() && serverName == (it + i)->getServName())
                    res = i;
            }
        }
    }
    return res;
}

/* Return empty srting if no error page for this number */
std::string ConfigInfo::getErrorPage(const int& servId, std::string locName, const int& errorNum) const {
    locName = getLocationByID(servId, locName);
    std::map<int, std::string> copyReturn;

    copyReturn = servers[servId].getLocations().find(locName)->second.errorPage;
    return copyReturn[errorNum];
}

std::string ConfigInfo::getRedirect(const int& servId, std::string locName, const int& redirectNum) const {
    locName = getLocationByID(servId, locName);
    std::map<int, std::string> copyReturn;

    copyReturn = servers[servId].getLocations().find(locName)->second.redirection;
    return copyReturn[redirectNum];
}

int ConfigInfo::getAutoIndex(const int& servId, std::string locName) const {
    locName = getLocationByID(servId, locName);
    return servers[servId].getLocations().find(locName)->second.autoIndex;
}

int ConfigInfo::getBodySize(const int& servId, std::string locName) const {
    locName = getLocationByID(servId, locName);
    return servers[servId].getLocations().find(locName)->second.bodySize;
}

std::string ConfigInfo::getRoot(const int& servId, std::string locName) const {
    locName = getLocationByID(servId, locName);
    return servers[servId].getLocations().find(locName)->second.root;
}

std::string ConfigInfo::getUploadPath(const int& servId, std::string locName) const {
    locName = getLocationByID( servId, locName );
    if(locName == "/")
        locName = servers[servId].getLocations().find( locName )->second.uploadPath;
    else
        locName += servers[servId].getLocations().find( locName )->second.uploadPath;
    return locName;
}

std::string ConfigInfo::getIndex(const int& servId, std::string locName) const {
    locName = getLocationByID(servId, locName);
    return servers[servId].getLocations().find(locName)->second.index;
}

std::vector<ServerConfig> ConfigInfo::getServers()const {
    return servers;
}

/* Private section */

void ConfigInfo::_identyServerValues(std::vector<std::string>& tokens) {
    std::vector<std::string>::iterator iter;
    while (tokens.size()) {
        iter = tokens.begin() + 1;
        while (iter != tokens.end() && *iter != "server")
            ++iter;
        servers.push_back(ServerConfig(std::vector<std::string>(tokens.begin() + 1, iter)));
        tokens.erase(tokens.begin(), iter);
    }
}

void ConfigInfo::_checkServNames() {
    std::vector<ServerConfig>::iterator it, itCheck, end;
    std::string nameToCheck;
    int res;

    it = servers.begin();
    end = servers.end();
    for (; it != end; ++it) {
        nameToCheck = it->getServName();
        if (nameToCheck.size()) {
            for (itCheck = it, ++itCheck; itCheck != end; ++itCheck) {
                if (itCheck->getServName() == nameToCheck)
                    throw utils::parseExeption("Error config: duplicate serv names");
            }
        }
    }
    res = NOT_FOUND;
    for (int i = 0; i < servers.size(); ++i) {
        res = checkHostPortDublicates(i);
        if (res != NOT_FOUND && servers[i].getServName() == servers[res].getServName())
            throw utils::parseExeption("Error config: duplicate host port without server_name");
    }
}
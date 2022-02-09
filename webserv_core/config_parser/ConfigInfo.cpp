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
    catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
    // std::cout << MAGENTA << "..........\n" << servers.size() << " servers successfully parsed" << RESET << std::endl;
    // std::cout << "redir " << getRedirect(0, "/c/t", i) << -1 << std::endl;
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
    std::size_t pos;

    if(servId < 0 || servId >= static_cast<int>(servers.size()) || locName.front() != '/') { // to debug reasons
        if(DEBUG_MODE)
            std::cout << BOLDRED << locName << " ServID = " << servId << RESET << std::endl;
        // std::cout << BOLDRED << "ERROR !!!!!!!\n\n\n\n ERROR !!!!!!!" // to debug reasons
        //     " \n getLocationByID = " << locName << "\n\n\nERROR !!!!!!!ServID = "  << servId << RESET << std::endl; // to debug reasons
        return "/";
    }// to debug reasons
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

    // std::cout << BOLDRED << 1 << RESET << std::endl;
    locName = getLocationByID( servId, locName );
    if (servId < 0)
        return false;
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

    // std::cout << BOLDRED << 2 << RESET << std::endl;
    locName = getLocationByID( servId, locName );
    std::map<int, std::string> copyReturn;

    if (servId < 0)
        return "";
    copyReturn = servers[servId].getLocations().find(locName)->second.errorPage;
    return copyReturn[errorNum];
}

std::string ConfigInfo::getRedirect(const int& servId, std::string locName, int& redirectNum) const {

    // std::cout << BOLDRED << 3 << RESET << std::endl;
    locName = getLocationByID( servId, locName );

    if (servId < 0)
        return "";
    redirectNum = servers[servId].getLocations().find(locName)->second.redirection.first;
    return servers[servId].getLocations().find(locName)->second.redirection.second;
}

int ConfigInfo::getAutoIndex(const int& servId, std::string locName) const {
    // std::cout << BOLDRED << 4 << RESET << std::endl;

    locName = getLocationByID( servId, locName );

    if (servId < 0)
        return 0;
    return servers[servId].getLocations().find( locName )->second.autoIndex;
}

int ConfigInfo::getBodySize(const int& servId, std::string locName) const {
    // std::cout << BOLDRED << 5 << RESET << std::endl;

    locName = getLocationByID( servId, locName );

    if (servId < 0)
        return 0;
    return servers[servId].getLocations().find( locName )->second.bodySize;
}

std::string ConfigInfo::getRootedUrl(const int& servId, std::string locName) const {
    // std::cout << BOLDRED << 6 << RESET << std::endl;

    std::string findLocName = getLocationByID( servId, locName );

    if (servId < 0)
        return "";
    if(findLocName == locName && servers[servId].getLocations().find( locName )->second.root.size())
        return servers[servId].getLocations().find(locName)->second.root;
    else
        return servers[servId].getLocations().find("/")->second.root + locName;
}

std::string ConfigInfo::getUploadPath(const int& servId, std::string locName) const {
    // std::cout << BOLDRED << 7 << RESET << std::endl;

    locName = getLocationByID( servId, locName );

    if (servId < 0)
        return "";
    locName = servers[servId].getLocations().find( locName )->second.uploadPath;
    return locName;
}

std::string ConfigInfo::getIndex(const int& servId, std::string locName) const {

    // std::cout << BOLDRED << 8 << RESET << std::endl;
    locName = getLocationByID( servId, locName );

    if (servId < 0)
        return "";
    return servers[servId].getLocations().find( locName )->second.index;
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
                    throw std::invalid_argument("Error config: duplicate serv names");
            }
            if (nameToCheck.find("/") != std::string::npos)
                throw std::invalid_argument("Error config: serv name with \"/\"");
        }
    }
    res = NOT_FOUND;
    for (std::size_t i = 0; i < servers.size(); ++i) {
        res = checkHostPortDublicates(i);
        if (res != NOT_FOUND && servers[i].getServName() == servers[res].getServName())
            throw std::invalid_argument("Error config: duplicate host port without server_name");
    }
}
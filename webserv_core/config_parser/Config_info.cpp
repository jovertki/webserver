#include "Config_info.hpp"

Config_info::Config_info(const char* arg) : servers() {
    std::ifstream input(arg);
    try {
        std::vector<std::string> tokens = utils::make_tokens(input);
        identyServerValues(tokens);
        checkServNames();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }

    //delete next line
    std::cout << "..........\n" << servers.size() << " servers successfully parsed" << std::endl;
//    std::cout << "ServID = " << getServerID("127.0.0.1", 8080, "cwd.localhost") << std::endl;
//    std::cout << "ServID = " << getServerID("127.0.0.1", 8080, "localhost") << std::endl;
//    std::cout << "ServID = " << getServerID("127.0.0.1", 8080) << std::endl;
//    std::cout << "ServID = " << getServerID("127.0.0.1", 8081) << std::endl;
//    std::cout << "ServID = " << getServerID("127.0.0.1", 4040) << std::endl;
//    std::cout << "ServID = " << getServerID("127.0.0.1", 4040, "localhost1") << std::endl;
//    std::cout << "metod is " << checkMethod(0, "/upload/find/some/", GET) << std::endl;
//    std::cout << "metod is " << checkMethod(0, "/hahah/", DELETE) << std::endl;
    std::cout << "autoindex is " << getAutoindex(0, "/upload/find") << std::endl;
    std::cout << "autoindex is " << getAutoindex(0, "/upload/find/") << std::endl;
    std::cout << "autoindex is " << getAutoindex(0, "/upload/find/3e3") << std::endl;


}

void Config_info::checkServNames() {
    std::vector<ServerConfig>::iterator it, itCheck, end;
    std::string nameToCheck;

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
}


/* Вернет -1 если сервера с данными параметрами не найдено
 * Вернет ID первого в массиве сервера с подходящими параметрами, если данные совпадают */
int Config_info::getServerID(std::string hostIP, int portVal, std::string serverName) {
    std::vector<ServerConfig>::iterator it, end;
    int res;

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

std::string Config_info::findLocationByID(const int servId, std::string locName) {
    int pos;

    while (locName.size() > 0) {
        if (servers[servId].getLocations().find(locName) != servers[servId].getLocations().end()) {
            return locName;
        }
        else {
            locName.pop_back();
            if ((pos = locName.find_last_of('/')) != std::string::npos)
                locName.resize(pos + 1);
        }
    }
    return locName;
}

bool Config_info::checkMethod(const int servId, std::string locName, const method methodForCheck) {
    std::vector<method> tempMethod;

    locName = findLocationByID(servId, locName);
    tempMethod = servers[servId].getLocations().find(locName)->second.methods;
    for (int i = 0; i + tempMethod.begin() < tempMethod.end(); ++i) {
        if (*(i + tempMethod.begin()) == methodForCheck)
            return true;
    }
    return false;
}

int Config_info::getAutoindex(const int servId, std::string locName) {
    locName = findLocationByID(servId, locName);
    return servers[servId].getLocations().find(locName)->second.autoindex;
}

void Config_info::identyServerValues(std::vector<std::string>& tokens) {
    std::vector<std::string>::iterator iter;
    while (tokens.size()) {
        iter = tokens.begin() + 1;
        while (iter != tokens.end() && *iter != "server")
            ++iter;
        servers.push_back(ServerConfig(std::vector<std::string>(tokens.begin() + 1, iter)));
        tokens.erase(tokens.begin(), iter);
    }
}

std::vector<ServerConfig> Config_info::get_servers()const {
    return servers;
}
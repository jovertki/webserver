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
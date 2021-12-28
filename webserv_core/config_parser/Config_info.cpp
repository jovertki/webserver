#include "Config_info.hpp"

Config_info::Config_info(const char* arg) : servers() {
    std::ifstream input(arg);
    if (!input.is_open()) {
        std::cout << "Error config: can't open file " << arg << std::endl;
        exit(-1);
    }
    try {
        std::vector<std::string> tokens = utils::make_tokens(input);
        identyServerValues(tokens);
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
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
//delete next line
    std::cout << "..........\n" << servers.size() << " servers successfully parsed" << std::endl;
}

std::vector<ServerConfig> Config_info::get_servers()const {
    return servers;
}
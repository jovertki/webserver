#include "Config_info.hpp"

Config_info::Config_info(const char* arg) : servers() {
    std::ifstream input(arg);
    if (!input.is_open()) {
        std::cout << "Error config: can't open file " << arg << std::endl;
        exit(-1);
    }
    std::vector<std::string> res = make_tokens(input);
    find_servers(res);
}

std::vector<std::string> Config_info::make_tokens(std::ifstream& input) {
    std::string oneToken;
    std::vector<std::string> tokens;
    char c;
    while (input.good())
    {
        c = input.get();
        if (std::isspace(c) || c == ';' || c == '\n' || c == EOF) {
           if (!oneToken.empty()) {
               tokens.push_back(std::string(oneToken));
               oneToken.clear();
           }
           if (c == ';')
               tokens.push_back(std::string(&c, 1));
        }
        else if (c == '#')
           input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        else
           oneToken.push_back(c);
    }
//    for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
//        std::cout << *iter << std::endl; // delete
    if (c != EOF || tokens.empty() || tokens.size() < 3) {
        std::cout << "Error config!" << std::endl;
        exit(-1); // make exeption
    }
    else
        input.close();
    return tokens;
}

void Config_info::find_servers(std::vector<std::string>& tokens) {
    std::vector<std::string>::iterator iter;
    while (tokens.size()) {
        iter = tokens.begin() + 1;
        while (iter != tokens.end() && *iter != "server")
            ++iter;
        servers.push_back(Server_info(std::vector<std::string>(tokens.begin() + 1, iter)));
        tokens.erase(tokens.begin(), iter);
    }
}

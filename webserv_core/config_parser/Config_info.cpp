#include "Config_info.hpp"

Config_info::Config_info(const char* arg) : servers() {
    std::ifstream input(arg); // std::ifstream::binary 2nd arg? // delete
    if (!input.is_open()) {
        std::cout << "Error config: can't open file " << arg << std::endl;
        exit(-1); // make some smart stuff
    }
    makeTokens(input);
    find_servers();
}


void Config_info::makeTokens(std::ifstream& input) {
    std::string oneToken;
    char c;
//    std::cout << oneToken << " Onetoken" << std::endl; // delete
    while (input.good())
    {
        c = input.get();
//        std::cout << ((int)c) << " last token "<< std::endl; // delete
        if (std::isspace(c) || c == ';' || c == '\n' || c == EOF) {
           if (!oneToken.empty()) {
               tokens.push_back(std::string(oneToken));
               oneToken.clear();
           }
//           if (c == ';') // exclude/include  ';' ? // delete
//               tokens.push_back(std::string(&c, 1));
        }
        else if (c == '#')
           input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        else
           oneToken.push_back(c);
    }
//    for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
//        std::cout << *iter << std::endl; // delete
    if (c != EOF) {
        std::cout << "Error config: EOF not reached!" << std::endl;
        exit(-1); // make some smart stuff
    }
    else if (tokens.empty()) {
        std::cout << "Error config: empty file!" << std::endl;
        exit(-1); // make some smart stuff
    }
    else if (tokens.size() < 3) {
        std::cout << "Error config: empty file!" << std::endl;
        exit(-1); // make some smart stuff
    }
    else
        input.close();
}

void Config_info::find_servers() {
    std::vector<std::string>::iterator iter;
    while (tokens.size()) {
        iter = tokens.begin() + 1;
        while (iter != tokens.end() && *iter != "server")
            ++iter;
        servers.push_back(Server_info(tokens.begin(), iter));
        tokens.erase(tokens.begin(), iter);
    }
}

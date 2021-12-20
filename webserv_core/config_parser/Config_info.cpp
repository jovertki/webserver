#include "Config_info.hpp"

Config_info::Config_info(const char* arg) : servers() {
    std::ifstream input(arg); // std::ifstream::binary 2nd arg? // delete
    if (!input.is_open()) {
        std::cout << "Error config: can't open file " << arg << std::endl;
        exit(-1);
    }
    std::vector<std::string> res = make_tokens(input);
    find_servers(res);
}

//std::vector<std::string> Config_info::make_lines(std::ifstream& input) {
//    std::string one_line;
//    std::vector<std::string> lines;
//
//    while(input.good())
//    {
//        std::getline(input, one_line);
//        if (one_line.size() && *one_line.begin() != '#')
//        {
//            while (std::isspace(*one_line.begin()))
//                one_line.erase(one_line.begin());
//            lines.push_back(std::string(one_line));
//        }
//        one_line.clear();
//    }
//    if (!input.eof()){
//        std::cout << "Error config: EOF not reached!" << std::endl;
//        exit(-1);
//    }
//    else
//        input.close();
//    return lines;
//    for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
//        std::cout << *iter << std::endl; // delete
}

//void Config_info::find_servers(std::vector<std::string>& lines) {
//    std::vector<std::string>::iterator iter;
//    while (lines.size()) {
//        iter = lines.begin();
//        if (*iter == "server {")
//        {
//            do
//                ++iter;
//            while ((iter != lines.end() && *iter != "server {"));
//            if ()
//            servers.push_back(Server_info(std::vector<std::string>(lines.begin() + 1, iter - 1)));
//            lines.erase(lines.begin(), iter);
//        }
//        else {
//            std::cout << "Error config description!" << std::endl;
//            exit(-1);
//        }
//    }
//}

//void Config_info::delete_lines() {
//    for (std::vector<std::string>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
//        std::cout << *iter << std::endl; // delete
//}

std::vector<std::string> Config_info::make_tokens(std::ifstream& input) {
    std::string oneToken;
    std::vector<std::string> tokens;
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
           if (c == ';') // exclude/include  ';' ? // delete
               tokens.push_back(std::string(&c, 1));
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
    else if (tokens.size() < 3) {  // how many minimum? check it here?
        std::cout << "Error config: empty file!" << std::endl;
        exit(-1); // make some smart stuff
    }
    else
        input.close();
    return tokens;
}



//
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

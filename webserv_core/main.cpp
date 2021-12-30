#include "servers/WebServer.hpp"
#include "config_parser/Config_info.hpp"


int main(int argc, char** argv, char **envp) {

    std::string conf_name = "configs/default.conf";
    if(argc == 2)
        conf_name = argv[1];
    if (argc > 2) {
        std::cout << "Wrong arguments" << std::endl;
        exit(-1);
    }
    Config_info config(conf_name.c_str());
        ft::WebServer t( envp, config);
}
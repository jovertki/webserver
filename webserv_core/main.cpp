#include "servers/WebServer.hpp"
#include "config_parser/ConfigInfo.hpp"

int main(int argc, char** argv, char **envp) {

    std::string conf_name = "configs/default.conf";

    if(argc == 2)
        conf_name = std::string(argv[1]);
    if (argc > 2) {
        std::cout << "Wrong arguments" << std::endl;
        exit(-1);
    }
    std::cout << BOLDMAGENTA << "CONFIG: " << conf_name << RESET << std::endl;
    ConfigInfo config(conf_name.c_str() );
    // system( "leaks webserv" );
    ft::WebServer t( envp, config );
}
#include "servers/TestServer.hpp"
#include "config_parser/Config_info.hpp"


int main(int argc, char** argv, char **envp) {

    if (argc == 2)
        Config_info config(argv[1]);
    else if (argc == 1)
        Config_info config("configs/default.conf");
    else {
        std::cout << "Wrong arguments" << std::endl;
        exit(-1);
    }
//	ft::TestServer t(envp);
}
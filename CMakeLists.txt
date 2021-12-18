cmake_minimum_required(VERSION 3.21)
project(webserver)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserver webserv_core/main.cpp
        webserv_core/utils/Request.cpp
        webserv_core/sockets/BindingSocket.cpp
        webserv_core/sockets/ListeningSocket.cpp
        webserv_core/sockets/SimpleSocket.cpp
        webserv_core/servers/SimpleServer.cpp
        webserv_core/servers/TestServer.cpp
        webserv_core/utils/Request.hpp
        webserv_core/sockets/BindingSocket.hpp
        webserv_core/sockets/ListeningSocket.hpp
        webserv_core/sockets/SimpleSocket.hpp
        webserv_core/servers/SimpleServer.hpp
        webserv_core/servers/TestServer.hpp
        webserv_core/config_parser/Server_info.cpp webserv_core/config_parser/Server_info.hpp webserv_core/config_parser/Config_info.cpp webserv_core/config_parser/Config_info.hpp)

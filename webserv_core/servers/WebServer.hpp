#pragma once
#include <string>
#include "../utils/Request.hpp"
#include <exception>
#include "../resources/defines.hpp"
#include "../config_parser/ConfigInfo.hpp"
#include "../sockets/ListeningSocket.hpp"
#include <sys/poll.h> //

namespace ft {
	class WebServer {
	private:
		class error_request_code : public std::exception {
			virtual const char* what() const throw();
		};
		std::vector<ListeningSocket> socket_array;
//		std::vector<ServerConfig> serverInfo;
		ConfigInfo config;
		char** envp;

		std::map<int, std::string> response_messeges;
	public:
		WebServer(char **envp, ConfigInfo &config);
		void launch( std::vector<pollfd>& fdset );
		int id;
		
	private:
		int accepter( int id );
		int handler( Request& );
		bool generate_regular_response( Request& );
		bool is_directory( const std::string& path )const;
		void list_contents( const std::string& path, Request& request )const;
		void handle_errors( int error_code, Request& request );
		bool response_POST( Request& request );
		bool response_GET( Request& request );
		bool response_DELETE( Request& request );
		bool execute_cgi( Request& request );
		void header_parse( const char*, Request& );
		char** create_appended_envp( Request& request );
		bool send_response( Request& )const;
		void init_response_msgs();
		void handle_multipart( Request& request, \
			char* buffer, long& bytes_read, std::ofstream& body_file);

		void check_new_clients( std::vector<pollfd>& fdset, std::map<int, Request>& requests );
		void work_with_clients( std::vector<pollfd>& fdset, std::map<int, Request>& requests );

		void respond( pollfd& fdset, Request& request );
		int recieve_request( pollfd& fdset, Request& request );

        _Noreturn void newest_global_loop( std::vector<pollfd>& fdset );
		std::vector<ListeningSocket> get_socket_array()const;
		int get_size_serverInfo() const;
		std::string generate_response_head( const int& code );
	};
}



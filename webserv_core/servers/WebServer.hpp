#pragma once
#include <string>
#include "../utils/Request.hpp"
#include "../resources/defines.hpp"
#include "../config_parser/ConfigInfo.hpp"
#include "../sockets/ListeningSocket.hpp"
#include <sys/poll.h> //
#include "../utils/Error_response_generator.hpp"

namespace ft {
	class WebServer {
	private:
		char** envp;
		ConfigInfo config;
		std::vector<ListeningSocket> socket_array;

		std::map<int, std::string> response_messeges;
		Error_response_generator error_handler;
	public:
		WebServer(char **envp, ConfigInfo &config);
		void launch( std::vector<pollfd>& fdset );
		int id;
		
	private:
		int accept_connection( const std::size_t& );
		bool generate_response( Request& );
		// bool is_directory( const std::string& path )const;
		void list_contents( const std::string& path, Request& request );
		void handle_errors( const int& error_code, Request& request);
		// bool response_POST( Request& request );
		bool response_GET_POST( Request& request );
		bool response_DELETE( Request& request );
		bool execute_cgi( Request& request );
		void header_parse( const char*, Request& );
		char** create_appended_envp( Request& request );
		int send_response( Request& )const;
		void init_response_msgs();
		void handle_multipart( Request& request, \
			char* buffer, long& bytes_read, std::ofstream& body_file);
		void write_response_to_file( Request& request );
		void check_new_clients( std::vector<pollfd>& fdset, std::map<int, Request>& requests );
		void work_with_clients( std::vector<pollfd>& fdset, std::map<int, Request>& requests );

		int get_serverID( Request& request );

		
		bool respond( pollfd& fdset, Request& request );
		int recieve_request( pollfd& fdset, Request& request );

        void global_loop( std::vector<pollfd>& fdset );
		std::vector<ListeningSocket> get_socket_array()const;
		int get_size_serverInfo() const;
		std::string generate_response_head( const int& code, Request& request );

		void generate_upload_response( Request& request );
		int remove_buffer_files( const int& fdset_fd );
		bool respond_out_of_line( Request& request, pollfd& fdset );
		void reset_request( pollfd& fdset, Request& request );
		void hard_close_connection( Request& request );
		void remove_hungup( std::vector<pollfd>& fdset, std::map<int, Request>& requests, const int& i );
		void recieve_avaliable( std::vector<pollfd>& fdset, std::map<int, Request>& requests, const int& i );
		void respond_avaliable( std::vector<pollfd>& fdset, std::map<int, Request>& requests, const int& i );
		void generate_redirect_response( const int& code, Request& request, const std::string& redirect_url );
	};
}



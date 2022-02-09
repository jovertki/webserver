#pragma once
#include <string>
#include "../resources/defines.hpp"
#include <map>
#include <vector>
#include <fstream>
#include "CGI_handler.hpp"
#include "Request_handler.hpp"
#include "../sockets/ListeningSocket.hpp"
#include <sys/poll.h>
//request stages
#define REQUEST_PENDING 0
#define REQUEST_FINISHED_READING 1
#define RESPONCE_GENERATED 2

namespace ft {
	class Request {
	private:
		int 			stage;

		const ListeningSocket* server_socket;
		int				servID;
		
		// int 			fd;
		int				method;
		std::string 	requested_url;
		std::string 	httpver;
		std::string 	query_string;
		std::map <std::string, std::string> params;
		std::string		rooted_url;

		
		std::string			cookie;
		CGI_handler 	cgi_handler;
		Request_handler rhandler;

		pollfd* fd_settings;
	public:
		bool cease_after_msg;
		std::string 	get_requested_filename() const;
		//used in sending response
		unsigned long lastPos;
		
		Request();
		// Request( const Request& a );
		// Request& operator=( const Request& a );
		int get_method() const;
		std::string get_requested_url() const;
		std::string get_httpver() const;
		std::string get_requested_url_extention() const;
		std::string get_content_type() const;
		std::string get_query_string() const;
		int get_fd() const;
		int get_servID() const;
		const ListeningSocket* get_socket()const;
		std::string get_serverIP()const;
		int get_serverPort()const;
		std::string get_serverName() const;
		std::string get_rooted_url() const;

		void	set_cookie( const std::string& );
		std::string get_cookie() const;
		std::string get_param_value( const std::string& n );
		
		void set_cgi( char** envp, const std::string& py_int = "", const std::string& pl_int = "");
		void set_request_handler();

		void set_socket( const ListeningSocket* );
		void set_rooted_url(const std::string&);
		void set_servID(const int&);
		void set_method( const int& );
		void set_requested_url(const std::string&);
		void set_httpver(const std::string&);
		void set_query_string( const std::string& );
		// void set_fd( const int& );
		void set_params( const std::map <std::string, std::string>& );
		void set_param( const std::string& key, const std::string& value );
		void set_fdset( pollfd* );
		void set_fd_events(const short& );


		
		void insert_param( const std::pair<std::string, std::string>& );
		void print_params()const;
		int param_exists( const std::string& ) const;
		
		bool is_pending() const;
		bool is_finished_reading() const;
		bool responce_is_generated() const;
		void set_stage( const int& );
		
		int execute_cgi();
		int execute_handler();


		bool is_chunked() const;
		void clear();
	};
}
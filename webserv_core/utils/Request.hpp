#pragma once
#include <string>
#include "../resources/defines.hpp"
#include <map>
#include <vector>
#include <fstream>
#include "CGI_handler.hpp"
#include "Request_handler.hpp"

//request stages
#define REQUEST_PENDING 0
#define REQUEST_FINISHED_READING 1
#define RESPONCE_GENERATED 2

namespace ft {
	class Request {
	private:
		int 			stage;
		
		int 			fd;
		int				method;
		std::string 	requested_url;
		std::string 	httpver;
		std::string 	query_string;
		std::map <std::string, std::string> params;
		
		CGI_handler 	cgi_handler;
		Request_handler rhandler;
		
		std::string 	get_requested_filename() const;
	public:
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
		// std::string get_param_value( const std::string& n );
		
		void set_cgi( char** envp );
		void set_request_handler();

		void set_method( const int& );
		void set_requested_url(const std::string&);
		void set_httpver(const std::string&);
		void set_query_string( const std::string& );
		void set_fd( const int& );
		void set_params( const std::map <std::string, std::string>& );
		void set_param( const std::string& key, const std::string& value );
		void insert_param( const std::pair<std::string, std::string>& );
		void print_params();
		// int param_exists( const std::string& ) const;

		bool is_pending() const;
		bool is_finished_reading() const;
		bool responce_is_generated() const;
		void set_stage( const int& );
		
		bool execute_cgi();
		int execute_handler();
		
		void clear();
	};
}
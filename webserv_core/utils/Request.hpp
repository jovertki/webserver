#pragma once
#include <string>
#include "../resources/defines.hpp"
#include <map>
#include <vector>
#include <fstream>
#include "CGI_handler.hpp"
#include "Request_handler.hpp"

namespace ft {
	class Request {
	private:
		//remain in request
		int method;
		std::string requested_url;
		std::string httpver;
		std::string query_string;
		std::string get_requested_filename() const;
	public:
		//remain in request
		int stage;
		int fd;
		CGI_handler cgi_handler;
		Request_handler rhandler;
		std::map <std::string, std::string> params;

		//used in sending response
		unsigned long lastPos;

		//remove candidate, used in handle multipart
		bool parsing_data_header;
		
		Request();
		// Request( const Request& a );
		// Request& operator=( const Request& a );
		int get_method() const;
		std::string get_requested_url() const;
		std::string get_httpver() const;
		std::string get_requested_url_extention() const;
		std::string get_content_type() const;
		std::string get_query_string() const;
		std::string get_param_value( const std::string& n );
		
		void set_cgi( char** envp );
		void set_request_handler();

		void set_method( const int& );
		void set_requested_url(const std::string&);
		void set_httpver(const std::string&);
		void set_query_string( const std::string& );
		
		void set_params( const std::map <std::string, std::string>& );
		void set_param( const std::string& key, const std::string& value );
		void insert_param( const std::pair<std::string, std::string>& );
		void print_params();
		int param_exists( const std::string& ) const;
	
		void clear();
	};
}
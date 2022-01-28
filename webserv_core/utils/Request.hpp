#pragma once
#include <string>
#include "../resources/defines.hpp"
#include <map>
#include <vector>
#include <fstream>
#include "CGI_handler.hpp"

namespace ft {
	class Request {
	private:
		//remain in request
		int method;
		std::string requested_url;
		std::string httpver;
		std::string query_string;
		std::string get_requested_filename() const;
		
		int header_length;

		long total_bytes_read;
		long full_request_length;
	public:
		//remain in request
		int stage;
		CGI_handler cgi_handler;
		int fd;
		std::map <std::string, std::string> params;

		unsigned long lastPos;

		
		bool parsing_header;
		bool parsing_data_header;
		Request();
		// Request( const Request& a );
		// Request& operator=( const Request& a );
		int get_method() const;
		std::string get_requested_url() const;
		std::string get_httpver() const;
		std::string get_requested_url_extention() const;
		std::string get_content_type() const;
		// std::ofstream get_body_fd() const;
		std::string get_query_string() const;
		std::string get_param_value( const std::string& n );
		int get_header_length()const;
		// std::map<std::string, std::string>::iterator get_params_begin() const;
		// std::map<std::string, std::string>::iterator get_params_end() const;
		long get_total_bytes_read() const;
		long get_full_request_length() const;
		
		void set_cgi(char** envp);
		void set_method( const int& );
		void set_requested_url(const std::string&);
		void set_httpver(const std::string&);
		// void set_body( const std::ofstream& );
		void set_query_string( const std::string& );
		// void set_body_args();
		void set_params( const std::map <std::string, std::string>& );
		void set_param( const std::string& key, const std::string& value );
		void set_header_length( const int& );
		void set_total_bytes_read( const long& );
		void set_full_request_length( const long& );
		
		void insert_param( const std::pair<std::string, std::string>& );
		void print_params();
		int param_exists( const std::string& ) const;
		void clear();
	};
}
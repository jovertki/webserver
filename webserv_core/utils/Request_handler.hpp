#pragma once
#include "../resources/defines.hpp"
#include <string>
#include <fstream>
#include <map>


namespace ft {
	class Request_handler {
	public:
		int execute();
		bool is_initialised();
		Request_handler( int* afd = NULL, int* amethod = NULL, \
			std::string* arurl = NULL, std::string* ahttpver = NULL, \
			std::map <std::string, std::string>* aparams = NULL, \
			std::string* aqstr = NULL );
	private:
		bool parsing_header;
		long full_request_length;
		long total_bytes_read;
		int header_length;

		//request stuff
		//content_length value
		int* fd;
		std::string *query_string;
		int* method;
		std::string* requested_url;
		std::string* httpver;
		std::map <std::string, std::string>* params;
		
		//insert param
		//set header length

		void handle_regular_body( int& current_pos, long& bytes_read, char buffer[], std::ofstream& body_file );
		int handle( long& bytes_read, char buffer[], std::ofstream& body_file );
		void header_parse( const char* input_buffer );
		int get_method( const std::string& token );
		void parse_query_string();
		int find_header_length( const char* input_buffer );
		void params_init( std::stringstream& ss );
		void open_file( std::ofstream& file );
		int new_bytes_to_read();
	};
}
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

		bool parsing_data_header;
		std::string multipart_boundary;
		
		int* fd;
		std::string *query_string;
		int* method;
		std::string* requested_url;
		std::string* httpver;
		std::map <std::string, std::string>* params;
		
		void open_file( std::ofstream& file );
		int new_bytes_to_read();
		
		int handle( long& bytes_read, std::string& buffer, std::ofstream& body_file );
		void header_parse( const std::string& input_buffer );
		void handle_regular_body( int& current_pos, long& bytes_read, const std::string& buffer, std::ofstream& body_file );


		bool body_exists();
		bool is_chunked();
		bool is_multipart( long& bytes_read );
		int get_method( const std::string& token );
		void parse_query_string();
		int find_header_length( const std::string& input_buffer );
		void params_init( std::stringstream& ss );
		void handle_multipart( std::string& buffer, long& bytes_read, std::ofstream& body_file );
		int multipart_parse_data_header( const std::string& buffer );
		void multipart_read_till_end_boundary( std::string& buffer, int& body_begin, \
			long& bytes_read, std::ofstream& body_file );
		std::string& set_boundary();
		bool boundary_is_found( const int& i, const long& bytes_read, const std::string& buffer );
	};
}
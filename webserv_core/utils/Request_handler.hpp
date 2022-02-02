#pragma once
#include "../resources/defines.hpp"
#include <string>
#include <fstream>
#include <map>
#include "../config_parser/Utils_parse_config.hpp"


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
        std::size_t chunkSize;
        std::size_t chunkRead;
        std::string end;

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
		
		int handle( std::string& buffer);
		void header_parse( const std::string& input_buffer );
		void handle_regular_body( const std::string& buffer, std::ofstream& body_file );

        bool parseChunkedBody(std::string& buffer);
		bool body_exists();
		bool is_chunked();
		bool is_multipart();
		int get_method( const std::string& token );
		void parse_query_string();
		int find_header_length( const std::string& input_buffer );
		void params_init( std::stringstream& ss );
		void handle_multipart( std::string& buffer);
		void multipart_parse_data_header( std::string& buffer );
		std::string& set_boundary();
		void delete_last_line( const long& file_len );
	};
}
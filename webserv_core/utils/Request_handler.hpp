#pragma once
#include "../resources/defines.hpp"
#include <string>
#include <fstream>
#include <map>
#include "../config_parser/Utils_parse_config.hpp"


namespace ft {
	class Request_handler {
	public:
		bool is_chunked()const;
		int execute();
		bool is_initialised();
		Request_handler( int* afd = NULL, int* amethod = NULL, \
			std::string* arurl = NULL, std::string* ahttpver = NULL, \
			std::map <std::string, std::string>* aparams = NULL, \
			std::string* aqstr = NULL );
	private:
		//request handler
		bool parsing_header;
		long full_request_length;
		long total_bytes_read;
		int header_length;

		//parsing chunked
		std::size_t chunkSize;
        std::size_t chunkRead;
        std::string end;

		//request data pointers
		int* fd;
		int* method;
		std::string* requested_url;
		std::string* httpver;
		std::map <std::string, std::string>* params;
		std::string *query_string;
		
		//multipart
		bool parsing_data_header;
		std::string multipart_boundary;

		int open_file( std::ofstream& file );
		int new_bytes_to_read();
		
		int handle( std::string& buffer);
		int header_parse( const std::string& input_buffer );
		int handle_regular_body( std::string& buffer, std::ofstream& body_file );

        bool parseChunkedBody(std::string& buffer);
		bool body_exists();
		bool is_multipart();
		int get_method( const std::string& token );
		void parse_query_string();
		static int find_header_length( const std::string& input_buffer );
		int params_init( std::stringstream& ss );
		void handle_multipart( std::string& buffer);
		void multipart_parse_data_header( std::string& buffer );
		std::string& set_boundary();
		void delete_last_line( const long& file_len );
	};
}
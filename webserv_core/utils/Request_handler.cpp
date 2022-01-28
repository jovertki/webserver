#include "Request_handler.hpp"
#include <unistd.h>
#include <sstream>
#include <iostream>
namespace ft {

	Request_handler::Request_handler( int* afd, int* amethod, std::string* arurl, \
		std::string* ahttpver, std::map <std::string, std::string>* aparams, std::string* aqstr ) \
		: parsing_header( true ), full_request_length( BUFFER_SIZE ), \
		total_bytes_read( 0 ), header_length( 0 ), fd( afd ), \
		method( amethod ), requested_url( arurl ), httpver( ahttpver ), \
		params( aparams ), query_string( aqstr ) {}

	bool Request_handler::is_initialised() {
		if(fd == NULL)
			return false;
		else
			return true;
	}
	
	int Request_handler::execute() {
		char buffer[BUFFER_SIZE + 1] = { 0 };
		int bytes_to_read;
		long bytes_read;
		bytes_to_read = new_bytes_to_read();
		bytes_read = read( *fd, buffer, bytes_to_read );
		if(bytes_read == -1) {
			return 0;//error connection
		}
		else if(bytes_read == 0) {//connection closed
			return 2;
		}
		else {
			std::ofstream body_file;
			open_file( body_file );
			int ret = handle( bytes_read, buffer, body_file );
			body_file.close();
			return ret;
		}
	}

	void Request_handler::open_file( std::ofstream& file ) {
		if(parsing_header) {
			//open anew
			file.open( BUFFER_FILE + std::to_string( *fd ), std::ios::binary );
		}
		else {
			//append
			file.open( BUFFER_FILE + std::to_string( *fd ), std::ios::binary | std::ios::app );
		}
		if(!file.is_open()) {
			//ERROR, WHICH ONE??
		}
	}

	int Request_handler::new_bytes_to_read() {
		if(full_request_length - total_bytes_read > BUFFER_SIZE || parsing_header) {
			return BUFFER_SIZE;
		}
		else {
			return( full_request_length - total_bytes_read);
		}
	}

	int Request_handler::get_method( const std::string& token ) {
		if(token == "GET") {
			return GET;
		}
		else if(token == "POST") {
			return POST;
		}
		else if(token == "DELETE") {
			return DELETE;
		}
		else {
			return EMPTY;
			//ERROR: INVALID METHOD
		}
	}
	
	void Request_handler::parse_query_string() {
		std::size_t questionmark = (*requested_url).find( "?" );
		if(questionmark != std::string::npos) {
			*query_string = (*requested_url).substr( questionmark + 1 );
			*requested_url = (*requested_url).substr( 0, questionmark );
		}
		else
			*query_string = "";
	}

	void Request_handler::params_init( std::stringstream& ss ) {
		std::string buffer;
		getline( ss, buffer ); //empty line
		getline( ss, buffer );//first line of headers
		while(buffer != "\r")
		{
			size_t colon = buffer.find( ':' );
			if(colon == std::string::npos) {
				//error in header
				break;
			}
			std::string key = buffer.substr( 0, colon );
			key = "HTTP_" + key;
			for(int i = 0; i < key.size(); i++) {
				if(key[i] == '-')
					key[i] = '_';
				key[i] = std::toupper( key[i] );
			}
			params->insert( make_pair( key, buffer.substr( colon + 2, buffer.size() - colon - 2 ) ) );
			// request.insert_param( make_pair( key, buffer.substr( colon + 2, buffer.size() - colon - 2 ) ) );
			if(!getline( ss, buffer )) {
				//ERROR INVALID REQUEST
			}

		}
	}
	int Request_handler::find_header_length( const char* input_buffer ) {
		std::string buffer_string( input_buffer );
		return buffer_string.find( "\r\n\r\n" );
	}
	
	void Request_handler::header_parse( const char* input_buffer ) {
		std::stringstream ss;
		ss << input_buffer;
		std::string token;
		ss >> token;
		*method = get_method( token );
		ss >> *requested_url;
		parse_query_string();
		ss >> *httpver;
		params_init( ss );
		header_length = find_header_length( input_buffer );
		// request.set_full_request_length( request.get_header_length() + 4 + atol( (request.get_param_value( "HTTP_CONTENT_LENGTH" )).c_str() ) );
		full_request_length = header_length + 4 + atol( (*params)["HTTP_CONTENT_LENGTH"].c_str() );//!!!!!!! POSSIBLE ERROR
		total_bytes_read = header_length + 4;
		if(DEBUG_MODE) {
			std::cout << "Method is " << *method << std::endl;
			std::cout << "URL is |" << *requested_url << "|" << std::endl;
			std::cout << "HTTPVER is |" << *httpver << "|" << std::endl;
		}
	}

	void Request_handler::handle_regular_body( int& begin_pos, long& bytes_read, char buffer[], std::ofstream& body_file ) {
		for(int i = begin_pos; i < bytes_read && total_bytes_read < full_request_length; i++) {
			body_file << buffer[i];
			total_bytes_read++;
		}
	}

	bool Request_handler::body_exists() {
		if((*params)["HTTP_CONTENT_LENGTH"] != "" || is_chunked()) {
			return true;
		}
		else
			return false;
	}

	bool Request_handler::is_chunked() {
		if((*params)["HTTP_TRANSFER_ENCODING"].find( "chunked" ) != std::string::npos) {
			return true;
		}
		else
			return false;
	}

	bool Request_handler::is_multipart( long& bytes_read ) {
		if((*params)["HTTP_CONTENT_TYPE"].find( "multipart/form-data" ) != std::string::npos \
			&& bytes_read > header_length + 4) {
			return true;
		}
		else
			return false;
	}
	int Request_handler::handle( long& bytes_read, char buffer[], std::ofstream& body_file ) {
		int begin_pos = 0;
		int is_over = 0;
		if(parsing_header) {
			header_parse( buffer );
			begin_pos = header_length + 4;
		}
		if(body_exists()) {
			if(is_chunked()) {
				// is_over = parseChunkedBody();//returns 1 if we have read everything, 0 otherwise
			}
			if(is_multipart( bytes_read )) {
				// handle_multipart( request, buffer, bytes_read, body_file );
			}
			handle_regular_body( begin_pos, bytes_read, buffer, body_file );
		}
		if((!is_chunked() && full_request_length - total_bytes_read <= 0) || is_over)//meaning we have read everything
			return 1;
		else
			return 0;
	}

	
}

// void Request_handler::handle_multipart( Request& request, \
// 	char* buffer, long& bytes_read, std::ofstream& body_file ) {
// 	std::string type = request.get_param_value( "HTTP_CONTENT_TYPE" );
// 	std::string boundary = type.substr( type.find( "boundary=" ) + 9 );
// 	boundary.insert( 0, "--" );
// 	boundary.insert( boundary.size(), "\0" );
// 	int i = 0;
// 	if(request.parsing_data_header) {
// 		std::string data_header;
// 		std::size_t data_header_end = 0;
// 		int data_header_begin = (request.get_header_length() + 4) * request.parsing_header;
// 		data_header.insert( 0, &buffer[data_header_begin] );
// 		if(data_header.size() != 0) {//there is body and this is not a first post request
// 			//request.print_params();
// 			if(DEBUG_MODE)
// 				std::cout << RED << "data header = " << data_header << RESET << std::endl;
// 			std::size_t filename_start = data_header.find( "filename=" );
// 			std::size_t filename_end = data_header.find( "\r\n", filename_start );
// 			std::string filename( data_header.substr( filename_start + 10, filename_end - filename_start - 11 ) );
// 			data_header_end = data_header.find( "\r\n\r\n" ) + 4;
// 			std::string upload_path = SERVER_DIR + std::string( "/uploads" ) + "/";
// 			filename.insert( 0, upload_path );
// 			if(DEBUG_MODE)
// 				std::cout << BLUE << filename << RESET << std::endl;
// 			std::cout << RED << "CHECK1" << data_header << RESET << std::endl;
// 			request.set_param( "UPLOAD_PATH", filename );
// 			std::cout << RED << "CHECK2" << data_header << RESET << std::endl;
// 		}
// 		i = (request.get_header_length() + 4) * request.parsing_header + data_header_end;
// 		request.set_total_bytes_read( request.get_total_bytes_read() + data_header_end );
// 		request.parsing_data_header = false;
// 	}
// 	for(; i < bytes_read && request.get_total_bytes_read() < request.get_full_request_length(); i++) {
// 		if(i + boundary.size() + 2 <= bytes_read) {
// 			if(buffer[i] == '\r') {
// 				if(strncmp( &buffer[i + 2], boundary.c_str(), boundary.size() - 1 ) == 0) {
// 					request.set_total_bytes_read( request.get_total_bytes_read() + boundary.size() + 5 );
// 					break;
// 				}
// 			}
// 			if(buffer[i] == '\n') {
// 				if(strncmp( &buffer[i + 1], boundary.c_str(), boundary.size() - 1 ) == 0) {
// 					request.set_total_bytes_read( request.get_total_bytes_read() + boundary.size() + 4 );
// 					break;
// 				}
// 			}
// 		}
// 		body_file << buffer[i];
// 		request.set_total_bytes_read( request.get_total_bytes_read() + 1 );
// 	}
// }
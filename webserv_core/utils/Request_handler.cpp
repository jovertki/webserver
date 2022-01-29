#include "Request_handler.hpp"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <vector>

namespace ft {

	Request_handler::Request_handler( int* afd, int* amethod, std::string* arurl, \
		std::string* ahttpver, std::map <std::string, std::string>* aparams, std::string* aqstr ) \
		: parsing_header( true ), full_request_length( BUFFER_SIZE ), \
		total_bytes_read( 0 ), header_length( 0 ), fd( afd ), \
		method( amethod ), requested_url( arurl ), httpver( ahttpver ), \
		params( aparams ), query_string( aqstr ), parsing_data_header( true ){}

	bool Request_handler::is_initialised() {
		if(fd == NULL)
			return false;
		else
			return true;
	}

	int Request_handler::execute() {
		char temp_buffer[BUFFER_SIZE + 1] = { 0 };
		int bytes_to_read = new_bytes_to_read();
		long bytes_read;

		bytes_read = read( *fd, temp_buffer, bytes_to_read );
		std::string buffer( temp_buffer, temp_buffer + bytes_read );//needs code review
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
			return(full_request_length - total_bytes_read);
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
	int Request_handler::find_header_length( const std::string& input_buffer ) {
		std::string buffer_string( input_buffer );
		return buffer_string.find( "\r\n\r\n" );
	}

	void Request_handler::header_parse( const std::string& input_buffer ) {
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
		full_request_length = header_length + 4 + atol( (*params)["HTTP_CONTENT_LENGTH"].c_str() );//!!!!!!! POSSIBLE ERROR
		total_bytes_read = header_length + 4;
		if(DEBUG_MODE) {
			std::cout << "Method is " << *method << std::endl;
			std::cout << "URL is |" << *requested_url << "|" << std::endl;
			std::cout << "HTTPVER is |" << *httpver << "|" << std::endl;
		}
	}

	void Request_handler::handle_regular_body( int& begin_pos, long& bytes_read, const std::string& buffer, std::ofstream& body_file ) {
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
	int Request_handler::handle( long& bytes_read, std::string& buffer, std::ofstream& body_file ) {
		int begin_pos = 0;
		int is_over = 0;
		if(parsing_header) {
			header_parse( buffer );
			begin_pos = header_length + 4;
			buffer.erase( buffer.begin(), buffer.begin() + begin_pos );
			parsing_header = false;
		}
		if(body_exists()) {
			if(is_chunked()) {
				 is_over = parseChunkedBody(buffer);//returns 1 if we have read everything, 0 otherwise
			}
			if(is_multipart( bytes_read )) {
				handle_multipart( buffer, bytes_read, body_file );//should delete transport stuff from buffer
			}
			handle_regular_body( begin_pos, bytes_read, buffer, body_file );
		}
		if((!is_chunked() && full_request_length - total_bytes_read <= 0) || is_over)//meaning we have read everything
			return 1;
		else
			return 0;
	}

    bool Request_handler::parseChunkedBody(std::string& buffer) {
        unsigned long startRN = 0;
        unsigned long position = 0;

        if (end.size()) { // some inf from last buffer
            buffer.insert(0, end);
            end.clear();
        }
        while (position < buffer.size()) {
            if (chunkSize == 0) { // find chunk
                if ((startRN = buffer.find("\r\n"), position) != std::string::npos) {
                    try { // before \r\n size of chunk
                        chunkSize = utils::strhex_to_num(buffer.substr(position, startRN)); }
                    catch (std::exception &e) {
                        std::cerr << e.what() << std::endl; }// mistake???
                    buffer.erase(position, startRN + 2); // delete \r\n
                    if (chunkSize == 0) //  all chunks done!!!
                        return true;
                }
                else { // \r\n and size can't be find so we need to wait next buffer
                    end = buffer.substr(position); // reminder
//                    std::cerr << "Error or real end buffer? END = " << end << std::endl; // delete!!!!!
                    buffer.erase(position);
                }
            }
            else {
                unsigned long needToRead = chunkSize - chunkRead;
                if (buffer.size() >= position + needToRead) { // if buffer more than current chunk
                    position += needToRead; // move to place were \r\n
                    if (buffer.size() - position < 2) { // if 1 or 0 symbols left after \r\n
                        end = buffer.substr(position);
                        buffer.erase(position);
                        chunkRead = chunkSize; //  in next func needToRead will be 0
                    }
                    else { // everything ok so only erase and return to first condition
                        buffer.erase(position, position + 2);
                        chunkSize = chunkRead = 0;
                    }
                }
                else { // not enough chunksize in buffer;
                    chunkRead += buffer.size() - position;
                    position = buffer.size(); //  stop while and  return false
                }
            }
        }
        return false;
    }


	int Request_handler::multipart_parse_data_header( const std::string& buffer ) {
		std::string data_header;
		std::size_t data_header_end = 0;
		int body_begin = 0;
		int data_header_begin = (header_length + 4) * parsing_header;
		data_header.insert( 0, &buffer[data_header_begin] );
		if(data_header.size() != 0) {//there is body and this is not a first post request
			//request.print_params();
			if(DEBUG_MODE)
				std::cout << RED << "data header = " << data_header << RESET << std::endl;
			std::size_t filename_start = data_header.find( "filename=" );
			std::size_t filename_end = data_header.find( "\r\n", filename_start );
			std::string filename( data_header.substr( filename_start + 10, filename_end - filename_start - 11 ) );
			data_header_end = data_header.find( "\r\n\r\n" ) + 4;
			std::string upload_path = SERVER_DIR + std::string( "/uploads" ) + "/";
			filename.insert( 0, upload_path );
			if(DEBUG_MODE)
				std::cout << BLUE << filename << RESET << std::endl;
			std::cout << RED << "CHECK1" << data_header << RESET << std::endl;
			(*params)["UPLOAD_PATH"] = filename;
			std::cout << RED << "CHECK2" << data_header << RESET << std::endl;
		}
		body_begin = (header_length + 4) * parsing_header + data_header_end;
		total_bytes_read += data_header_end;
		parsing_data_header = false;
		return body_begin;
	}

	std::string& Request_handler::set_boundary() {
		std::string type = (*params)["HTTP_CONTENT_TYPE"];
		multipart_boundary = type.substr( type.find( "boundary=" ) + 9 );
		multipart_boundary.insert( 0, "--" );
		multipart_boundary.insert( multipart_boundary.size(), "\0" );
		return multipart_boundary;
	}

	bool Request_handler::boundary_is_found( const int& i, const long& bytes_read, const std::string& buffer ) {
		if(i + multipart_boundary.size() + 2 <= bytes_read) {
			if(buffer[i] == '\r') {
				if(strncmp( &buffer[i + 2], multipart_boundary.c_str(), multipart_boundary.size() - 1 ) == 0) {
					total_bytes_read += multipart_boundary.size() + 5;
					return 1;
				}
			}
			if(buffer[i] == '\n') {
				if(strncmp( &buffer[i + 1], multipart_boundary.c_str(), multipart_boundary.size() - 1 ) == 0) {
					total_bytes_read += multipart_boundary.size() + 4;
					return 1;
				}
			}
		}
		return 0;
	}
	void Request_handler::multipart_read_till_end_boundary( std::string& buffer, int& body_begin, long& bytes_read, std::ofstream& body_file ) {
		if(multipart_boundary == "") {
			multipart_boundary = set_boundary();
		}
		for(int i = body_begin; i < bytes_read && total_bytes_read < full_request_length; i++) {
			if(boundary_is_found( i, bytes_read, buffer )) {
				break;
			}
			body_file << buffer[i];
			total_bytes_read++;
		}
	}
	
	void Request_handler::handle_multipart( std::string& buffer, long& bytes_read, std::ofstream& body_file ) {
		int body_begin = 0;
		if(parsing_data_header) {
			body_begin = multipart_parse_data_header( buffer);
			parsing_data_header = false;
		}
		multipart_read_till_end_boundary(buffer, body_begin, bytes_read, body_file);
		
	}
}
#include "Request.hpp"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>

//#define MIME_FILE "../resources/mime.types"
// #define BUFFER_SIZE 30000 //is always bigger then 8000, max HTTP header size
//buffer size defined not here
ft::Request::Request() {
	clear();
	parsing_data_header = true;
	stage = REQUEST_PENDING;
	
	// needToReturn = 0;
	lastPos = 0;
	fd = -1;
}

void ft::Request::set_cgi(char** envp) {
	cgi_handler = CGI_handler( envp, &fd, &requested_url, \
		& query_string, &method, &params );
}

void ft::Request::set_request_handler() {
	if(!rhandler.is_initialised()){
		rhandler = Request_handler( &fd, &method, &requested_url, \
			& httpver, &params, &query_string );
	}

}
// ft::Request::Request( const ft::Request& a ) : method( a.method ), requested_url( a.requested_url ), httpver( a.httpver ), \
// header_length( a.header_length ), query_string( a.query_string ), params( a.params ), \
// total_bytes_read( a.total_bytes_read ), full_request_length( a.full_request_length ) {
// 	a.response_file.close();
// 	response_file.open( BUFFER_FILE_OUT + std::to_string( fd ) );
// }

// ft::Request& ft::Request::operator=( const ft::Request& a ) {
// 	if(this != &a) {
// 		method = a.method;
// 		requested_url = a.requested_url;
// 		httpver = a.httpver;
// 		header_length = a.header_length;
// 		query_string = a.query_string;
// 		params = a.params;
// 		total_bytes_read = a.total_bytes_read;
// 		full_request_length = a.full_request_length;
// 		response_is_ready = a.response_is_ready;
// 		fd = a.fd;
// 		parsing_header = a.parsing_header;
// 		parsing_data_header = a.parsing_data_header;
// 		response_file.open( BUFFER_FILE_OUT + std::to_string( fd ) );
// 	}
// 	return *this;
// }


int ft::Request::get_method() const {
	return method;
}

std::string ft::Request::get_requested_url() const {
	return requested_url;
}
std::string ft::Request::get_httpver() const {
	return httpver;
}

std::string ft::Request::get_requested_filename() const {
	return requested_url.substr( requested_url.find_last_of( "/" ) );
}

std::string ft::Request::get_requested_url_extention() const {
	std::size_t last_dot = get_requested_filename().find_last_of( "." );
	if(last_dot == std::string::npos)
		return "html";
	else

		return get_requested_filename().substr( last_dot + 1 );
}

std::string ft::Request::get_content_type() const {
	std::ifstream infile( MIME_FILE );
	if(infile.is_open())
	{
		std::string line;
		while(std::getline( infile, line ))
		{
			std::istringstream iss( line );
			std::string content_type, extention;
			iss >> content_type;
			while(iss >> extention) {
				if(extention == get_requested_url_extention())
					return content_type;
			}
		}
	}
	return "no such type";
}

std::string ft::Request::get_query_string() const {
	return query_string;
}

void ft::Request::set_method(const int &n ) {
	method = n;
}
void ft::Request::set_requested_url( const std::string& n ) {
	requested_url = n;
}
void ft::Request::set_httpver( const std::string& n ) {
	httpver = n;
}

void ft::Request::set_query_string( const std::string& n ) {
	query_string.insert(query_string.begin(), n.begin(), n.end());
}

void ft::Request::set_params( const std::map <std::string, std::string>& n ) {
	params = n;
}


void ft::Request::insert_param( const std::pair<std::string, std::string>& n ) {
	params.insert( n );

}

void ft::Request::print_params() {
	//debug output
	for(std::map<std::string, std::string>::const_iterator i = params.begin(); i != params.end(); i++) {
		std::cout << MAGENTA << ( *i ).first << ":" << (*i).second <<RESET<< std::endl;
	}
}

int ft::Request::param_exists( const std::string& n) const {
	if(params.find( n ) != params.end())
		return 1;
	else
		return 0;
}

std::string ft::Request::get_param_value( const std::string& n ) {
	if(param_exists( n )) {
		return params[n];
	}
	else
		return "";
}

void ft::Request::clear() {
	method = 0;
	requested_url = "";
	httpver = "";
	query_string.clear();
	params.clear();
	cgi_handler = CGI_handler();
}

void ft::Request::set_param( const std::string& key, const std::string& value ) {
	params[key] = value;
}
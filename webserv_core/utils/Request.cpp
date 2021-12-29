#include "Request.hpp"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
//#define MIME_FILE "../resources/mime.types"

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

// std::ofstream ft::Request::get_body_fd() const {
// 	return body_file;
// }

std::string ft::Request::get_query_string() const {
	return query_string;
}

std::map<std::string, std::string> ft::Request::get_params()const {
	return params;
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

// void ft::Request::set_body( const std::ofstream& n ) {
// 	body_file = n;
// }


void ft::Request::set_query_string( const std::string& n ) {
	query_string.insert(query_string.begin(), n.begin(), n.end());
}
// void ft::Request::set_body_args() {
// 	query_string = get_body();
// }

void ft::Request::set_params( const std::map <std::string, std::string>& n ) {
	params = n;
}


void ft::Request::insert_param( const std::pair<std::string, std::string>& n ) {
	params.insert( n );

}

void ft::Request::print_params() {
	//debug output
	for(std::map<std::string, std::string>::const_iterator i = params.begin(); i != params.end(); i++) {
		std::cout << (*i).first << ":" << (*i).second << std::endl;
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

void ft::Request::set_header_length( const int& n) {
	header_length = n;
}

int ft::Request::get_header_length()const {
	return header_length;
}

std::map<std::string, std::string>::iterator ft::Request::get_params_begin() {
	return params.begin();
}
std::map<std::string, std::string>::iterator ft::Request::get_params_end() {
	return params.end();
}

void ft::Request::clear() {
	method = 0;
	requested_url = "";
	httpver = "";
	header_length = 0;
	// body_file.close();
	std::remove( BUFFER_FILE );
	query_string.clear();
	params.clear();
}
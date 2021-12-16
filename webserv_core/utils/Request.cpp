#include "Request.hpp"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>

#define MIME_FILE "resources/mime.types"
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

std::string ft::Request::get_body() const {
	return body;
}

std::string ft::Request::get_args() const {
	return args;
}

void ft::Request::set_method( int n ) {
	method = n;
}
void ft::Request::set_requested_url( std::string n) {
	requested_url = n;
}
void ft::Request::set_httpver( std::string n) {
	httpver = n;
}

void ft::Request::set_body( std::string n) {
	body = n;
}


void ft::Request::set_url_args( std::string n) {
	args = n;
}
void ft::Request::set_body_args() {
	args = get_body();
}
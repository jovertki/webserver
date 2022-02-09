#include "CGI_handler.hpp"
#include <fcntl.h>
#include <fstream>
#include "Request.hpp"
#include "../resources/defines.hpp"
#include <sstream>
#include <iostream>
#include <signal.h>

//cgi stages
#define CGI_NOT_STARTED 0
#define CGI_PROCESSING 1
#define CGI_FINISHED 2


ft::CGI_handler::~CGI_handler(){

}

bool ft::CGI_handler::is_initialised() {
	if(envp == NULL) {
		return false;
	}
	else
		return true;
}

ft::CGI_handler::CGI_handler( char** envp, int* afd, \
	 std::string* arequested_url,  std::string* aquery_string, \
	int* amethod, std::map <std::string, std::string>* aparams, \
	const std::string& py_int, const std::string& pl_int, std::string* arooted_url ) : envp( envp ), stage( CGI_NOT_STARTED ), \
	cgi_pid( -1 ), fd( afd ), requested_url( arequested_url ), \
	query_string( aquery_string ), method( amethod ), params( aparams ), rooted_url(arooted_url), \
	python_interpretator( py_int ), perl_interpretator( pl_int ){
	init_response_msgs();
}

int ft::CGI_handler::execute() {
	int error_code = 0;
	if(stage == CGI_NOT_STARTED) {
		this->start();
	}
	if(stage == CGI_PROCESSING) {
		error_code = this->process();
		if(error_code) {
			return error_code;
		}
	}
	if (stage == CGI_FINISHED){
		error_code = this->write();
		if(error_code) {
			return error_code;
		}
		return 1;
	}
	return 0;
}

void ft::CGI_handler::init_new_envp( std::map<std::string, std::string>& additions) {
	additions["REQUEST_METHOD"] = "";
	additions["PATH_INFO"] = "";
	additions["AUTH_TYPE"] = "";//not used
	additions["CONTENT_LENGTH"] = std::to_string( get_bodyfile_length() );
	additions["CONTENT_TYPE"] = "";
	additions["GATEWAY_INTERFACE"] = "";//not used
	additions["PATH_TRANSLATED"] = "";//not used
	additions["QUERY_STRING"] = "";
	additions["REMOTE_ADDR"] = "";//not used
	additions["REMOTE_HOST"] = "";
	additions["REMOTE_IDENT"] = "";//not used
	additions["REMOTE_USER"] = "";//not used
	additions["SCRIPT_NAME"] = "";
	additions["SERVER_NAME"] = "";//NYI
	additions["SERVER_PORT"] = "";//NYI
	additions["SERVER_PROTOCOL"] = "";
	additions["SERVER_SOFTWARE"] = "";//not used


	if(*method == GET)
		additions["REQUEST_METHOD"] = "GET";
	else if(*method == POST)
		additions["REQUEST_METHOD"] = "POST";
	else if(*method == DELETE)
		additions["REQUEST_METHOD"] = "DELETE";

	additions["PATH_INFO"] = *requested_url;
	additions["QUERY_STRING"] = *query_string;
	additions["SCRIPT_NAME"] = additions["PATH_INFO"];
	additions["SERVER_PROTOCOL"] = "HTTP/1.1\0";

	additions.insert( params->begin(), params->end() );

	additions["REMOTE_HOST"] = additions["HTTP_HOST"];
	additions["CONTENT_TYPE"] = additions["HTTP_CONTENT_TYPE"];

	additions["SERVER_NAME"] = "";//NYI
	additions["SERVER_PORT"] = "";//NYI



}


char** ft::CGI_handler::create_appended_envp(){
	std::map<std::string, std::string> additions;
	init_new_envp(additions);

	int envp_len = 0;
	while(envp[envp_len] != NULL) {
		envp_len++;
	}

	int new_envp_len = envp_len + additions.size() + 1;
	char** new_envp = new char* [new_envp_len];
	for(int i = 0; i < new_envp_len; i++) {
		new_envp[i] = NULL;
	}

	int cur = 0;
	for(int i = 0; envp[i] != NULL; i++) {
		new_envp[cur] = strdup( envp[i] );
		cur++;
	}

	for(std::map<std::string, std::string>::iterator it = additions.begin(); it != additions.end(); it++) {
		std::stringstream ss;
		ss << it->first << "=" << it->second << "\0";
		new_envp[cur] = strdup(ss.str().c_str());
		cur++;
	}
	return new_envp;
}

void ft::CGI_handler::execute_extention_script( const std::string& filename, char** cgi_envp ) {
	char** args = new char* [3];
	args[1] = new char[filename.size() + 1];
	bzero( args[1], filename.size() + 1 );
	strcpy( args[1], filename.c_str() );
	args[2] = NULL;
	std::string interpreter;
	if(get_extention() == "py") {
		interpreter = python_interpretator;
	}
	else if(get_extention() == "pl") {
		interpreter = perl_interpretator;
	}
	args[0] = new char[interpreter.size() + 1];
	bzero( args[0], interpreter.size() + 1 );
	strcpy( args[0], interpreter.data() );
	if(execve( interpreter.c_str(), args, cgi_envp ) == -1) {
		// std::cout << RED << "execute_script: " << strerror( errno ) << RESET << std::endl;
		exit( 404 );
	}
}

void ft::CGI_handler::execute_script() {//needs beauty
	char** cgi_envp = create_appended_envp();
	std::string filename = *rooted_url;
	if(get_extention() != "") {
		execute_extention_script( filename, cgi_envp );
	}
	else {//execute binary
		if(execve( filename.c_str(), NULL, cgi_envp ) == -1) {
			// std::cout << RED << "execute_script: " << strerror( errno ) << RESET << std::endl;
			exit( 404 );
		}
	}
}

void ft::CGI_handler::start(){
	stage = CGI_PROCESSING;
	int body_fd = open( (BUFFER_FILE + std::to_string( *fd )).c_str(), O_RDONLY );
	int response_file_fd = open( (BUFFER_FILE_CGIOUT + std::to_string( *fd )).c_str(), O_WRONLY | O_CREAT, 0666 );
	// std::cout << RED << "cgi extention is |" << get_extention() << "|" << RESET << std::endl;

	if(DEBUG_MODE) {
		for(std::map <std::string, std::string>::iterator i = params->begin(); i != params->end(); i++) {
			std::cout << MAGENTA << (*i).first << " " << (*i).second << RESET << std::endl;
		}
		std::cout << MAGENTA << *query_string << RESET << std::endl << std::endl;
		std::cout << RED << "rooted_url cgi = " << *rooted_url << RESET << std::endl;
	}
	pid_t ret = fork();
	if(ret == 0)
	{
		dup2( body_fd, 0 );
		dup2( response_file_fd, 1 );

		execute_script();
		// std::cout << "ERRRPR" << std::endl;
		// std::cout << strerror( errno ) << std::endl;
		exit( 404 );
	}
	else {
		cgi_pid = ret;
		close( response_file_fd );
		close( body_fd );
	}
}

int ft::CGI_handler::process() {
	int status;
	waitpid( cgi_pid, &status, WNOHANG );
	if(kill( cgi_pid, 0 ) == -1) {
		stage = CGI_FINISHED;
		if(WEXITSTATUS( status ) != 0)
			return WEXITSTATUS( status );
	}
	return 0;
}


long ft::CGI_handler::find_cgi_file_length( std::ifstream& cgi_response_file, std::string& content_type ) {
	cgi_response_file.seekg( 0, std::ios::end );
	long cgi_file_length = cgi_response_file.tellg();
	cgi_file_length -= (content_type.size() + 4);
	cgi_response_file.close();
	cgi_response_file.open( BUFFER_FILE_CGIOUT + std::to_string( *fd ), std::ios::binary );
	return cgi_file_length;
}

int ft::CGI_handler::write() {
	std::ifstream cgi_response_file;
	cgi_response_file.open( BUFFER_FILE_CGIOUT + std::to_string( *fd ) );
	if(!cgi_response_file.is_open())
		return 500;
		
	std::string content_type;
	std::getline( cgi_response_file, content_type );
	content_type.pop_back();//delete extra /r
	// std::cout << RED << content_type << content_type.size() << " " << strlen( "Content-Type: text/html" ) << RESET << std::endl;

	long cgi_file_length = find_cgi_file_length( cgi_response_file, content_type );


	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( *fd ), std::ios::binary );
	if(!response_file.is_open()) {
		return 500;
	}
	response_file << generate_response_head( 200 ) << "Content-Length: " << cgi_file_length << "\r\n";

	char buffer[CGI_BUFFER_SIZE + 1];
	while(!cgi_response_file.eof()) {
		bzero( buffer, CGI_BUFFER_SIZE );
		cgi_response_file.read( buffer, CGI_BUFFER_SIZE );
		response_file.write( buffer, cgi_response_file.gcount() );
	}
	cgi_response_file.close();
	std::remove( (BUFFER_FILE_CGIOUT + std::to_string( *fd )).c_str() );
	response_file.close();
	return 0;
}

std::string ft::CGI_handler::generate_response_head( const int& code ) {
	std::stringstream ss;
	ss << "HTTP/1.1 " << code << " " << response_messeges[code] << "\r\n";
	return ss.str();
}

void ft::CGI_handler::init_response_msgs() {
	response_messeges[100] = "Continue";

	response_messeges[200] = "OK";
	response_messeges[202] = "Accepted";
	response_messeges[204] = "No Content";
	response_messeges[206] = "Partial Content";
	response_messeges[226] = "IM used";

	response_messeges[300] = "Multiple Choices";
	response_messeges[303] = "See Other";
	response_messeges[308] = "Permanent Redirect";

	response_messeges[400] = "Bad Request";
	response_messeges[403] = "Forbidden";
	response_messeges[404] = "Not Found";
	response_messeges[405] = "Method Not Allowed";
	response_messeges[406] = "Not Acceptable";
	response_messeges[408] = "Request Timeout";
	response_messeges[410] = "Gone";
	response_messeges[411] = "Length Required";
	response_messeges[413] = "Payload Too Large";
	response_messeges[414] = "URI Too Long";
	response_messeges[415] = "Unsupported Media Type";
	response_messeges[416] = "Range Not Satisfiable";
	response_messeges[417] = "Expectation Failed";
	response_messeges[421] = "Misdirected Request";
	response_messeges[422] = "Unprocessable Entity";
	response_messeges[423] = "Locked";
	response_messeges[424] = "Failed Dependency";
	response_messeges[429] = "Too Many Requests";
	response_messeges[431] = "Request Header Fields Too Large";

	response_messeges[500] = "Internal Server Error";
	response_messeges[501] = "Not Implemented";
	response_messeges[502] = "Bad Gateway";
	response_messeges[503] = "Service Unavailable";
	response_messeges[504] = "Gateway Timeout";
	response_messeges[505] = "HTTP Version Not Supported";
	response_messeges[507] = "Insufficient Storage";
	response_messeges[508] = "Loop Detected";
	response_messeges[510] = "Not Extended ";
}


std::string ft::CGI_handler::get_extention()const {
	std::size_t dot = requested_url->find_last_of( "." );
	if(dot == std::string::npos)
		return "";
	else
		return requested_url->substr( dot + 1, requested_url->size() - dot );
}

int ft::CGI_handler::get_bodyfile_length() {
	std::ifstream body_file;
	body_file.open( BUFFER_FILE + std::to_string( *fd ), std::ios::binary );
	body_file.seekg( 0, std::ios::end );
	long ret = body_file.tellg();
	body_file.close();
	return ret;

}
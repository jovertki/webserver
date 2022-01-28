#include "CGI_handler.hpp"
#include <fcntl.h>
#include <fstream>
#include "Request.hpp"
#include "../resources/defines.hpp"
#include <sstream>
#include <iostream>
#include <signal.h>


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
	int* amethod, std::map <std::string, std::string>* aparams ) : envp( envp ), stage( CGI_NOT_STARTED ), cgi_pid( -1 ), \
	fd( afd ), requested_url( arequested_url ), query_string( aquery_string ), method( amethod ), params( aparams ) {
	init_response_msgs();
}

bool ft::CGI_handler::execute() {
	if (stage == CGI_NOT_STARTED){
		this->start();

	}
	if (stage == CGI_PROCESSING){
		this->process();
	}
	if (stage == CGI_FINISHED){
		this->write();
		return true;
	}
	return false;
}

void ft::CGI_handler::init_new_envp( std::map<std::string, std::string>& additions) {
	additions["REQUEST_METHOD"] = "";
	additions["PATH_INFO"] = "";
	additions["AUTH_TYPE"] = "";//not used
	additions["CONTENT_LENGTH"] = "";//needs elaboration
	additions["CONTENT_TYPE"] = "";
	additions["GATEWAY_INTERFACE"] = "";//not used
	additions["PATH_TRANSLATED"] = "";//not used
	additions["QUERY_STRING"] = "";
	additions["REMOTE_ADDR"] = "";//not used
	additions["REMOTE_HOST"] = "";
	additions["REMOTE_IDENT"] = "";//not used
	additions["REMOTE_USER"] = "";//not used
	additions["REQUEST_METHOD"] = "";
	additions["SCRIPT_NAME"] = "";//not perfect
	additions["SERVER_NAME"] = "";//NYI
	additions["SERVER_PORT"] = "";//NYI
	additions["SERVER_PROTOCOL"] = "";
	additions["SERVER_SOFTWARE"] = "";//not used
	// additions["UPLOAD_PATH"] = "";//NYI


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


void ft::CGI_handler::start(){
	stage = CGI_PROCESSING;
	char** cgi_envp = create_appended_envp();
	int body_fd = open( (BUFFER_FILE + std::to_string( *fd )).c_str(), O_RDONLY );
	int response_file_fd = open( (BUFFER_FILE_CGIOUT + std::to_string( *fd )).c_str(), O_WRONLY | O_CREAT, 0666 );
	std::string debug = SERVER_DIR + *requested_url;
	pid_t ret = fork();
	if(ret == 0)
	{
		dup2( body_fd, 0 );
		dup2( response_file_fd, 1 );
		std::string filename = SERVER_DIR + *requested_url;
		execve( filename.c_str(), NULL, cgi_envp );
		std::cout << "ERRRPR" << std::endl;
		std::cout << strerror( errno ) << std::endl;
		exit( 234 );
	}
	else {
		cgi_pid = ret;
		close( response_file_fd );
		close( body_fd );
	}
}

void ft::CGI_handler::process(){
	waitpid( cgi_pid, NULL, WNOHANG );
	// request->cgi_stage = CGI_FINISHED;
	if(kill( cgi_pid, 0 ) == -1) {
		stage = CGI_FINISHED;
	}
}

void ft::CGI_handler::write(){
	std::ifstream cgi_response_file;
	cgi_response_file.open( BUFFER_FILE_CGIOUT + std::to_string( *fd ) );
	if(!cgi_response_file.is_open())
		std::cout << RED << strerror( errno ) << RESET << std::endl;
	std::string content_type;
	std::getline( cgi_response_file, content_type );
	std::cout << RED << content_type << content_type.size() << " " << strlen( "Content-Type: text/html" ) << RESET << std::endl;

	cgi_response_file.seekg( 0, std::ios::end );
	long cgi_file_length = cgi_response_file.tellg();

	cgi_response_file.close();

	cgi_response_file.open( BUFFER_FILE_CGIOUT + std::to_string( *fd ), std::ios::binary );


	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( *fd ), std::ios::binary );
	response_file << generate_response_head( 200 ) << "Content-Length:" << std::to_string( cgi_file_length - content_type.size() - 4 ) << "\r\n";

	char buffer[CGI_BUFFER_SIZE + 1];
	while(!cgi_response_file.eof()) {
		bzero( buffer, CGI_BUFFER_SIZE );
		cgi_response_file.read( buffer, CGI_BUFFER_SIZE );
		response_file.write( buffer, cgi_response_file.gcount() );
	}
	cgi_response_file.close();
	std::remove( (BUFFER_FILE_CGIOUT + std::to_string( *fd )).c_str() );
	response_file.close();
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
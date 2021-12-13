#include "TestServer.hpp"
#include <cstring>
#include <string>
#include "../resources/lex_defines.h"
#include <fstream>

#define SERVER_DIR "/Users/jovertki/school21projects/webserv_project/webserv_core/html"
ft::TestServer::TestServer(char **envp) : SimpleServer( AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10 ), envp(envp) {
	bzero( buffer, 30000 );
	launch();
}

void ft::TestServer::accepter() {
	struct sockaddr_in address = get_socket()->get_address();
	int addrlen = sizeof( address );
	new_socket = accept( get_socket()->get_sock(), (struct sockaddr*)&address, (socklen_t*)&addrlen );
	long end = read( new_socket, buffer, 30000 );
	buffer[end] = '\0';
	buffer_s = buffer;

	//fix windows endlines
	std::size_t n = buffer_s.find( (char)13 );
	while(n != std::string::npos) {
		buffer_s.replace( n, 1, "" );
		n = buffer_s.find( (char)13 );
	}
}

void ft::TestServer::handler() {
	std::cout << "buffer is \n" << buffer_s << std::endl;
	std::cout << "first is \"" << buffer_s[0] << "\"" << std::endl;
	std::string line = buffer_s.substr( 0, buffer_s.find( "\n" ) );
	std::cout << "line is \n" << line << std::endl;

	//HTTP request parser
	//find method
	std::size_t endline = line.find( " " );
	if(line.substr( 0, endline ) == "GET") {
		request.set_method(GET);
	}
	else if(line.substr( 0, endline ) == "POST") {
		request.set_method( POST );
	}
	std::cout << "Method is " << request.get_method() << std::endl;


	//find url
	std::size_t new_endline = line.find( " ", endline + 1 );
	request.set_requested_url( line.substr( endline + 1, new_endline - endline - 1 ) );
	//find args in url
	std::size_t questionmark = request.get_requested_url().find( "?" );
	if(questionmark != std::string::npos) {
		request.set_url_args( request.get_requested_url().substr( questionmark + 1 ) );
		request.set_requested_url( request.get_requested_url().substr( 0, questionmark ) );
	}
	else
		request.set_url_args( "" );
	std::cout << "URL is |" << request.get_requested_url() << "|" << std::endl;

	//find httpver
	request.set_httpver( line.substr( new_endline + 1, line.size() - (new_endline + 1) ) );
	std::cout << "HTTPVER is |" << request.get_httpver() << "|" << std::endl;


	//find body
	if(buffer_s.find( "\n\n", 0, 2 ) != std::string::npos)
		request.set_body( buffer_s.substr( buffer_s.find( "\n\n" ) + 2 ) );
	std::cout << "BODY is |" << request.get_body() << "|" << std::endl;

	//set args to body if method is POST
	if (request.get_method() == POST){
		request.set_body_args();
	}
	std::cout << "ARGS is |" << request.get_args() << "|" << std::endl;
}
void ft::TestServer::response_post() {
	std::cout << "========RESPONSE POST IS ACTIVE========" << std::endl;
	int ret = fork();
	if(ret == 0)
	{
		std::cout << "Trying to execute " << request.get_requested_filename() << std::endl;
		std::string filename = SERVER_DIR + request.get_requested_filename();
		
		if(execve( filename.c_str(), NULL, envp ) < 0)
			std::cout << "ERRRPR" << std::endl;
		exit( 234 );
	}
}
void ft::TestServer::response_get() {
	std::string hello;
	//first response line
	hello = hello + request.get_httpver() + " 200 OK\n";

	//check if file exists
	//check if rights are correct

	//response header
	if(request.get_requested_url() == "/")
		request.set_requested_url("/index.html");

	std::string content_type = request.get_content_type();

	//read file to string
	std::ifstream infile( SERVER_DIR + request.get_requested_url() );
	std::string content( (std::istreambuf_iterator<char>( infile )),
		(std::istreambuf_iterator<char>()) );

	//write file to string
	hello = hello + "Content-Type: " + content_type + ";\nContent-Length:" + std::to_string( content.size() ) + "\n\n" + content;

	//write string to socket
	write( new_socket, hello.c_str(), hello.size() );
	std::cout << "RESPONSE IS \n" << hello << "===end of response===" << std::endl;
	close( new_socket );
}

void ft::TestServer::responder() {


	//pack appropriate stuff to GET_RESPONSE function
	//create a responder for every request-type
	if(request.get_method() == GET)
		response_get();
	else if(request.get_method() == POST)
		response_post();

	
}

void ft::TestServer::launch() {
	while(true) {
		std::cout << "waiting" << std::endl;
		accepter();
		handler();
		responder();
		std::cout << "==== DONE ====" << std::endl;
	}
}
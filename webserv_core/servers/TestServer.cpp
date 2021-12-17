#include "TestServer.hpp"
#include <cstring>
#include <string>
#include "../resources/lex_defines.h"
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define SERVER_DIR "/Users/jovertki/webserver_shared_repo/webserv_core/html"
#define ERROR404FILE "./html/error404.html"




const char* ft::TestServer::error_request_code::what() const throw(){
	return ("error");
}

ft::TestServer::TestServer( char** envp ) : SimpleServer( AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10 ), envp( envp ) {
	launch();
}

void ft::TestServer::accepter() {
	struct sockaddr_in address = get_socket()->get_address();
	int addrlen = sizeof( address );
	new_socket = accept( get_socket()->get_sock(), (struct sockaddr*)&address, (socklen_t*)&addrlen );
	long end = read( new_socket, buffer, 30000 );
	if(end != 0)
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

	char** my_envp = new char* [10];

	std::string myWord = "REQUEST_METHOD=POST";
	my_envp[0] = new char[myWord.size() + 1];
	strcpy( my_envp[0], myWord.c_str() );
	myWord = "SERVER_PROTOCOL=HTTP/1.1";
	my_envp[1] = new char[myWord.size() + 1];
	strcpy( my_envp[1], myWord.c_str() );
	myWord = "PATH_INFO=/Users/jovertki/webserver_shared_repo/webserv_core/html/cgi_tester";
	my_envp[2] = new char[myWord.size() + 1];
	strcpy( my_envp[2], myWord.c_str() );
	myWord = "PATH=/Users/jovertki/.brew/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/share/dotnet:/usr/local/munk";
	my_envp[3] = new char[myWord.size() + 1];
	strcpy( my_envp[3], myWord.c_str() );

	myWord = "ARGS=" + request.get_args();
	my_envp[4] = new char[myWord.size() + 1];
	strcpy( my_envp[4], myWord.c_str() );
	my_envp[5] = NULL;
	
	int fdpipe[2];
	pipe( fdpipe );
	int ret = fork();
	if(ret == 0)
	{
		dup2( fdpipe[1], 1 );
		//std::cout << "Trying to execute " << request.get_requested_filename() << std::endl;
		std::string filename = SERVER_DIR + request.get_requested_filename();
		execve( filename.c_str(), NULL, my_envp );
		std::cout << "ERRRPR" << std::endl;
		exit( 234 );
	}
	else {
		waitpid( ret, NULL, 0 );
	}
	char buff[30000] = { 0 };
	int len = read( fdpipe[0], buff, 30000 ) - strlen( "Content-Type: text/html\n\n" );
	close( fdpipe[0] );
	close( fdpipe[1] );
	std::cout << "BUFF IS " << buff << std::endl;
	std::string out = request.get_httpver() + " 200 OK\n" + "Content-Length:" + std::to_string( len ) + "\n" + static_cast<std::string>(buff);
	for(int i = 0; i < 6; i++) {
		delete my_envp[i];
	}
	delete[] my_envp;

	write( new_socket, out.c_str(), out.size() );
	std::cout << "RESPONSE IS \n" << out << "===end of response===" << std::endl;
	close( new_socket );
}

void ft::TestServer::response_get() {

	//check if rights are correct

	//response header
	if(request.get_requested_url() == "/")
		request.set_requested_url("/index.html");


	std::string response;
	if(is_directory( SERVER_DIR + request.get_requested_url() ) /*&& AUTOINDEX IS ON*/) {
	//list contents
		response = list_contents( SERVER_DIR + request.get_requested_url() );
	}
	else {
	//proceed with request
		std::string content_type = request.get_content_type();

		//read file to string
		std::ifstream infile( SERVER_DIR + request.get_requested_url() );
		if(!infile.is_open()) {
			handle_errors( 404 );
		}
		std::string content( (std::istreambuf_iterator<char>( infile )),
			(std::istreambuf_iterator<char>()) );

		//first response line
		response = response + request.get_httpver() + " 200 OK\n";
		//write file to string
		response = response + "Content-Type: " + content_type + ";\nContent-Length:" + std::to_string( content.size() ) + "\n\n" + content;
	}
	//write string to socket
	write( new_socket, response.c_str(), response.size() );
	std::cout << "RESPONSE IS \n" << response << "===end of response===" << std::endl;
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


bool ft::TestServer::is_directory( const std::string& path )const {
	struct stat s;
	if(stat( path.c_str(), &s ) == 0)
	{
		if(s.st_mode & S_IFDIR)
		{
			//it's a directory
			return 1;
		}
		else if(s.st_mode & S_IFREG)
		{
			//it's a file
			return 0;
		}
		else
		{
			//something else
			return 0;
		}
	}
	else
	{
		//error
	}
	return 0;
}
void ft::TestServer::handle_errors( int error_code ) {
	std::string response;
	std::ostringstream header;
	std::ostringstream body;

	body <<	"<!DOCTYPE html>" << std::endl << \
		"<html lang=\"en\">" << std::endl << std::endl << \
		"<head>" << std::endl << \
		"<title>"<< error_code << " Error Page</title>" << std::endl << std::endl << \
		"<meta charset=\"utf-8\">" << std::endl << \
		"<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl << \
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl << \
		"<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->" << std::endl << std::endl << \
		"<!-- Bootstrap -->" << std::endl << std::endl << \
		"<link rel = \"stylesheet\" href = \"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\">" << std::endl << std::endl << \
		"<!-- Custom stlylesheet -->" << std::endl << \
		"<link type=\"text/css\" rel=\"stylesheet\" href=\"css/style.css\" />" << std::endl << std::endl << \
		"</head>" << std::endl << std::endl << \
		"<body>" << std::endl << \
		"<div class=\"vertical-center\">" << std::endl << \
		"<div class=\"container\">" << std::endl << \
		"<div id=\"notfound\" class=\"text-center\">" << std::endl << \
		"<h1>😮</h1>"  <<std::endl << \
		"<h1>" << error_code << "</h1>"  << std::endl << \
		"<p> An error occured.</p>" << std::endl << \
		"<a href=\"/index.html\">Back to homepage</a>" << std::endl << \
		"</div>" << std::endl << \
		"</div>" << std::endl << \
		"</div>" << std::endl << \
		"</body>" << std::endl << \
		"</html>" << std::endl;
	
	header << request.get_httpver() << " " << error_code << " " << "KO" << std::endl <<//<- needs elaboration
		"Content-Type: text/html;" << std::endl << \
		"Content-Length: " << body.str().size() << std::endl << std::endl;
	response = header.str() + body.str();
	write( new_socket, response.c_str(), response.size() );
	std::cout << "RESPONSE IS \n" << response << "===end of response===" << std::endl;
	close( new_socket );
	throw (error_request_code());
}
std::string ft::TestServer::list_contents(const std::string& path)const {
	std::string response;
	std::ostringstream header;
	std::ostringstream body;

	body << "<!DOCTYPE html>" << std::endl << \
		"<html lang=\"en\">" << std::endl << std::endl << \
		"<head>" << std::endl << \
		"<title>Folder contents</title>" << std::endl << std::endl << \
		"<meta charset=\"utf-8\">" << std::endl << \
		"<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl << \
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl << \
		"<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->" << std::endl << std::endl << \
		"<!-- Bootstrap -->" << std::endl << std::endl << \
		"<link rel = \"stylesheet\" href = \"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\">" << std::endl << std::endl << \
		"<!-- Custom stlylesheet -->" << std::endl << \
		"<link type=\"text/css\" rel=\"stylesheet\" href=\"/css/listing.css\" />" << std::endl << std::endl << \
		"</head>" << std::endl << std::endl << \
		"<body>" << std::endl << \
		"<h1>Contents\n\n</h1> " << std::endl;


	DIR* dir;
	struct dirent* ent;
	if((dir = opendir( path.c_str() )) != NULL) {
		/* print all the files and directories within directory */
		ent = readdir( dir );
		while((ent = readdir( dir )) != NULL) {
			body << "<a href=\"" << request.get_requested_url();
			if(request.get_requested_url()[request.get_requested_url().size() - 1] != '/')
				body << "/";
			body << ent->d_name << "\">" << ent->d_name << "</a><br>";
		}
		closedir( dir );
	}
	else {
		/* could not open directory */
	}
	
	body <<	"</body>" << std::endl << \
		"</html>" << std::endl;

	header << request.get_httpver() << " 200 " << "OK" << std::endl <<
		"Content-Type: text/html;" << std::endl << \
		"Content-Length: " << body.str().size() << std::endl << std::endl;
	response = header.str() + body.str();
	return response;
}
void ft::TestServer::launch() {
	while(true) {
		std::cout << "waiting" << std::endl;
		try {
			//poll will be here
			accepter();
			handler();
			responder();
		}
		catch(error_request_code& e) {}
		std::cout << "==== DONE ====" << std::endl;
	}
}
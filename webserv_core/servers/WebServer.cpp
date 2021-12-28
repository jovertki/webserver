#include "WebServer.hpp"
#include <cstring>
#include <string>
// #include "../resources/lex_defines.h"
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstdio>

#include <errno.h>

const char* ft::WebServer::error_request_code::what() const throw() {
	return ("error");
}

ft::WebServer::WebServer( char** envp, Config_info &config) : envp( envp ), config(config.get_servers()){
	socket = new ft::ListeningSocket( AF_INET, SOCK_STREAM, 0, 4242, INADDR_ANY, 10 );
	launch();
}

void ft::WebServer::accepter() {
	char buffer[30001] = { 0 };
	std::string buffer_s;
	struct sockaddr_in address = get_socket()->get_address();
	int addrlen = sizeof( address );
	long content_length;
	new_socket = accept( get_socket()->get_sock(), (struct sockaddr*)&address, (socklen_t*)&addrlen );

	//READ LOOP HERE
	long end = recv( new_socket, buffer, 30000, 0 );
	buffer_s = buffer;
	std::cout << "buffer is \n" << buffer_s << std::endl;

	//parse head, if not full in buffer, then error
	//use map for storage
	header_parse( buffer_s );

	//debug only
	std::ofstream last_request( "last_request.txt" );
	last_request << buffer_s;

	
	content_length = atol( (request.get_param_value( "Content-length" )).c_str() );
	//read body for content-length bytes


	
	//find body
	request.set_body( buffer_s.substr( buffer_s.find( "\r\n\r\n" ) + 4 ) );

	std::cout << "BODY is |\n";
	for(int i = 0; i < request.get_body().size(); ++i)
		std::cout << request.get_body()[i];
	std::cout << "\n|" << std::endl;

	//set args to body if method is POST
	if(request.get_method() == POST) {
		request.set_body_args();
	}
	std::cout << "ARGS is |\n";
		for(int i = 0; i < request.get_args().size(); ++i)
			std::cout << request.get_args()[i];
	std::cout << "\n|" << std::endl;




	last_request.close();
	
	//fix windows endlines
	// std::size_t n = buffer_s.find( (char)13 );
	// while(n != std::string::npos) {
	// 	buffer_s.replace( n, 1, "" );
	// 	n = buffer_s.find( (char)13 );
	// }
}

void ft::WebServer::header_parse(std::string& buffer_s) {
	
	// std::string line = buffer_s.substr( 0, buffer_s.find( "\n" ) );
	std::stringstream ss;
	ss << buffer_s;
	std::string token;
	ss >> token;
	//HTTP request parser
	//find method
	// std::size_t endline = line.find( " " );
	if(token == "GET") {
		request.set_method( GET );
	}
	else if(token == "POST") {
		request.set_method( POST );
	}
	else if(token == "DELETE") {
		request.set_method( DELETE );
	}
	else {
		//ERROR: INVALID METHOD
	}
	std::cout << "Method is " << request.get_method() << std::endl;

	//find url
	ss >> token;
	request.set_requested_url( token );
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
	ss >> token;
	request.set_httpver( token );
	std::cout << "HTTPVER is |" << request.get_httpver() << "|" << std::endl;
	
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
		request.insert_param( make_pair( buffer.substr( 0, colon ), buffer.substr( colon + 2, buffer.size() - colon - 2 ) ) );
		if(!getline( ss, buffer )) {
			//ERROR INVALID REQUEST
		}
			
	}
	request.set_header_length( buffer_s.find( "\r\n\r\n" ) );

	//debug only
	request.print_params();

}

void ft::WebServer::handler() {
	
}
void ft::WebServer::response_POST() {
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

	myWord = "SOCKET=" + std::to_string( new_socket );
	my_envp[4] = new char[myWord.size() + 1];
	strcpy( my_envp[4], myWord.c_str() );

	myWord = "ARGS=";
	for(int i = 0; i < request.get_args().size(); ++i)
		myWord += request.get_args()[i];
	myWord += "\0";
	my_envp[5] = new char[myWord.size() + 1];
	strcpy( my_envp[5], myWord.c_str() );


	my_envp[6] = NULL;

	int fdpipe[2];
	int fdpipein[2];
	pipe( fdpipe );
	pipe( fdpipein );
	std::string str = "MY String for cgi script\n";
	write( fdpipein[1], str.c_str(), str.size() );
	int ret = fork();
	if(ret == 0)
	{
		dup2( fdpipein[0], 0 );
		dup2( fdpipe[1], 1 );
		//std::cout << "Trying to execute " << request.get_requested_filename() << std::endl;
		std::string filename = SERVER_DIR + request.get_requested_url();
		execve( filename.c_str(), NULL, my_envp );
		std::cout << "ERRRPR" << std::endl;
		std::cout << strerror( errno ) << std::endl;
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
	std::cout << "RESPONSE IS \n" << out << "\n===end of response===" << std::endl;
	close( new_socket );
}

void ft::WebServer::response_GET() {

	//check if rights are correct

	//response header
	if(request.get_requested_url() == "/")
		request.set_requested_url( "/index.html" );


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
	std::cout << "RESPONSE IS \n" << response << "\n===end of response===" << std::endl;
	close( new_socket );
}

void ft::WebServer::response_DELETE() {
	//do smth
	// if(is_directory( SERVER_DIR + request.get_requested_url() ) /*&& AUTOINDEX IS ON*/) {
	// 	//list contents
	// 	response = list_contents( SERVER_DIR + request.get_requested_url() );
	// }

	std::string response;
	std::ostringstream sresponse;
	if(std::remove( (SERVER_DIR + request.get_requested_url()).c_str() ) < 0) {
		//error
	}
	else {
		sresponse << "HTTP/1.1 200 OK\n\nFile " << request.get_requested_url() << " was successfully DELETED" << std::endl;
	}
	response = sresponse.str();
	write( new_socket, response.c_str(), response.size() );
	std::cout << "RESPONSE IS \n" << response << "\n===end of response===" << std::endl;
	close( new_socket );
}

void ft::WebServer::responder() {
	//check availability of method in location
	if(request.get_method() == GET)
		response_GET();
	else if(request.get_method() == POST)
		response_POST();
	else if(request.get_method() == DELETE)
		response_DELETE();
}


bool ft::WebServer::is_directory( const std::string& path )const {
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
void ft::WebServer::handle_errors( int error_code ) {
	std::string response;
	std::ostringstream header;
	std::ostringstream body;

	body << "<!DOCTYPE html>" << std::endl << \
		"<html lang=\"en\">" << std::endl << std::endl << \
		"<head>" << std::endl << \
		"<title>" << error_code << " Error Page</title>" << std::endl << std::endl << \
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
		"<h1>:-(</h1>"   <<std::endl << \
		"<h1>" << error_code << "</h1>" << std::endl << \
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
std::string ft::WebServer::list_contents( const std::string& path )const {
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

	body << "</body>" << std::endl << \
		"</html>" << std::endl;

	header << request.get_httpver() << " 200 " << "OK" << std::endl <<
		"Content-Type: text/html;" << std::endl << \
		"Content-Length: " << body.str().size() << std::endl << std::endl;
	response = header.str() + body.str();
	return response;
}
void ft::WebServer::launch() {
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


ft::ListeningSocket* ft::WebServer::get_socket()const {
	return socket;
}
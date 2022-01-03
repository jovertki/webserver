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
#include <cctype>
#include <errno.h>
#include <cstdlib>
#include <sstream>
#include <fcntl.h>

#define DEBUG_MODE 1
#define BUFFER_SIZE 30000

const char* ft::WebServer::error_request_code::what() const throw() {
	return ("error");
}

ft::WebServer::WebServer( char** envp, Config_info &config) : envp( envp ), config(config.get_servers()){
	socket = new ft::ListeningSocket( AF_INET, SOCK_STREAM, 0, 4242, INADDR_ANY, 10 );
	init_response_msgs();
	launch();
}


void ft::WebServer::handle_multipart( Request& request, \
	char* buffer, long& bytes_read, std::ofstream& body_file, \
	long& total_bytes_read, long& end_of_read, bool& parsing_data_header, bool header_included ) {
	std::string type = request.get_param_value( "HTTP_CONTENT_TYPE" );
	std::string boundary = type.substr( type.find( "boundary=" ) + 9 );
	boundary.insert( 0, "--" );
	boundary.insert( boundary.size(), "\0" );
	int i = 0;
	if(parsing_data_header) {
		std::string data_header;
		std::size_t data_header_end = 0;
		int data_header_begin = (request.get_header_length() + 4) * header_included;
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
			request.set_param( "UPLOAD_PATH", filename );
		}
		i = (request.get_header_length() + 4) * header_included + data_header_end;
		total_bytes_read += data_header_end;
		parsing_data_header = false;
	}
	for(; i < bytes_read && total_bytes_read < end_of_read; i++) {
		if(i + boundary.size() + 2 <= bytes_read) {
			if(buffer[i] == '\r') {
				if(strncmp( &buffer[i + 2], boundary.c_str(), boundary.size() - 1 ) == 0) {
					total_bytes_read += boundary.size() + 5;
					break;
				}
			}
			if(buffer[i] == '\n') {
				if(strncmp( &buffer[i + 1], boundary.c_str(), boundary.size() - 1 ) == 0) {
					total_bytes_read += boundary.size() + 4;
					break;
				}
			}
		}
		body_file << buffer[i];
		total_bytes_read++;
	}
}

void ft::WebServer::accepter( Request& request ) {
	struct sockaddr_in address = get_socket()->get_address();
	int addrlen = sizeof( address );
	new_socket = accept( get_socket()->get_sock(), (struct sockaddr*)&address, (socklen_t*)&addrlen );

}

void ft::WebServer::header_parse( const char* input_buffer, Request& request ) {
	
	// std::string line = buffer_string.substr( 0, buffer_string.find( "\n" ) );
	std::stringstream ss;
	ss << input_buffer;
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
	if(DEBUG_MODE)
		std::cout << "Method is " << request.get_method() << std::endl;

	//find url
	ss >> token;
	request.set_requested_url( token );
	//find args in url
	std::size_t questionmark = request.get_requested_url().find( "?" );
	if(questionmark != std::string::npos) {
		request.set_query_string( request.get_requested_url().substr( questionmark + 1 ) );
		request.set_requested_url( request.get_requested_url().substr( 0, questionmark ) );
	}
	else
		request.set_query_string( "" );
	if(DEBUG_MODE)
		std::cout << "URL is |" << request.get_requested_url() << "|" << std::endl;

	//find httpver
	ss >> token;
	request.set_httpver( token );
	if(DEBUG_MODE)
		std::cout << "HTTPVER is |" << request.get_httpver() << "|" << std::endl;

	//fill params with http request headers
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
		request.insert_param( make_pair( key, buffer.substr( colon + 2, buffer.size() - colon - 2 ) ) );
		if(!getline( ss, buffer )) {
			//ERROR INVALID REQUEST
		}
			
	}
	std::string buffer_string( input_buffer );
	request.set_header_length( buffer_string.find( "\r\n\r\n" ) );

	//debug only
	//request.print_params();

}

void ft::WebServer::handler( Request& request ) {
	char buffer[BUFFER_SIZE + 1] = { 0 };
	bool parsing_header = true;
	int bytes_to_read = BUFFER_SIZE;
	long content_length;
	long end_of_read = BUFFER_SIZE;
	long total_bytes_read = 0;
	long bytes_read = 1;
	bool parsing_data_header = true;
	std::ofstream body_file( BUFFER_FILE, std::ios::binary );
	std::ofstream last_request( "last_request.txt" );

	for(bytes_read = recv( new_socket, buffer, bytes_to_read, 0 );\
		bytes_read != 0 && end_of_read - total_bytes_read > 0; \
		bytes_read = recv( new_socket, buffer, bytes_to_read, 0 )) {
		if(DEBUG_MODE) {//print buffer
			std::cout << YELLOW << "buffer is \n";
			for(int i = 0; i < bytes_read; i++) {
				std::cout << buffer[i];
			}
			std::cout << "bytes read = " << bytes_read << RESET << std::endl;
		}

		int i = 0;
		if(parsing_header) {
			if(bytes_read == 0)
				throw error_request_code();
			//parse head, if not full in buffer, then error
		//use map for storage
			header_parse( buffer, request );
			content_length = atol( (request.get_param_value( "HTTP_CONTENT_LENGTH" )).c_str() );
			end_of_read = request.get_header_length() + 4 + content_length;
			total_bytes_read = request.get_header_length() + 4;
			i = request.get_header_length() + 4;
		}

		if(DEBUG_MODE) {
			last_request << buffer;
		}

		if(!body_file.is_open()) {
			//ERROR, WHICH ONE??
		}
		if(request.get_param_value( "HTTP_CONTENT_TYPE" ).find( "multipart/form-data" ) != std::string::npos \
			&& bytes_read != request.get_header_length() + 4) { //meaning file is being uploaded
			handle_multipart( request, buffer, bytes_read, body_file, total_bytes_read, end_of_read, parsing_data_header, parsing_header);
			parsing_header = false;
		}
		else {
			for(; i < bytes_read && total_bytes_read < end_of_read; i++) {
				body_file << buffer[i];
				total_bytes_read++;
			}
			parsing_header = false;
		}
		if(end_of_read - total_bytes_read > BUFFER_SIZE) {
			bytes_to_read = BUFFER_SIZE;
		}
		else {
			bytes_to_read = end_of_read - total_bytes_read;
		}
		bzero( buffer, BUFFER_SIZE );
	}
	//find body
	body_file.close();


	//debug only
	if(DEBUG_MODE) {
		last_request.close();
		std::cout << "QUERY_STRING is |\n";
		for(int i = 0; i < request.get_query_string().size(); ++i)
			std::cout << request.get_query_string()[i];
		std::cout << "\n|" << std::endl;
	}

}



void ft::WebServer::init_new_envp( std::map<std::string, std::string>& additions, Request& request ) {
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


	if(request.get_method() == GET)
		additions["REQUEST_METHOD"] = "GET";
	else if(request.get_method() == POST)
		additions["REQUEST_METHOD"] = "POST";
	else if(request.get_method() == DELETE)
		additions["REQUEST_METHOD"] = "DELETE";

	additions["PATH_INFO"] = request.get_requested_url();
	additions["QUERY_STRING"] = request.get_query_string();
	additions["SCRIPT_NAME"] = additions["PATH_INFO"];
	additions["SERVER_PROTOCOL"] = "HTTP/1.1\0";
	
	additions.insert( request.get_params_begin(), request.get_params_end() );

	additions["REMOTE_HOST"] = additions["HTTP_HOST"];
	additions["CONTENT_TYPE"] = additions["HTTP_CONTENT_TYPE"];

	additions["SERVER_NAME"] = "";//NYI
	additions["SERVER_PORT"] = "";//NYI


	

}

char** ft::WebServer::create_appended_envp( Request& request ) {
	std::map<std::string, std::string> additions;
	init_new_envp( additions, request);
	
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

void ft::WebServer::response_POST( Request& request ) {
	std::cout << "========RESPONSE POST IS ACTIVE========" << std::endl;

	if(request.get_requested_url().find( "/cgi-bin/" ) == 0 && request.get_requested_url().size() > 9 /*sizeof( "/cgi-bin/" )*/) { //if it is in /cgi-bin/
		execute_cgi( request );
	}

	
	// write( new_socket, out.c_str(), out.size() );
	// std::cout << GREEN<< "===RESPONSE BEGIN===\n" << out << "\n===RESPONCE END===" << RESET << std::endl;
	// close( new_socket );
}


void ft::WebServer::execute_cgi( Request& request ) {
	char** cgi_envp = create_appended_envp( request );

	int fdpipe[2];
	pipe( fdpipe );

	int body_fd = open( BUFFER_FILE, O_RDONLY );
	int ret = fork();
	if(ret == 0)
	{
		dup2( body_fd, 0 );
		dup2( fdpipe[1], 1 );
		//std::cout << "Trying to execute " << request.get_requested_filename() << std::endl;
		std::string filename = SERVER_DIR + request.get_requested_url();
		execve( filename.c_str(), NULL, cgi_envp );
		std::cout << "ERRRPR" << std::endl;
		std::cout << strerror( errno ) << std::endl;
		exit( 234 );
	}
	else {
		close( fdpipe[1] );
		waitpid( ret, NULL, 0 );
	}
	char buff[30001] = { 0 };
	//should be in a loop
	int len = read( fdpipe[0], buff, 30000 ) - strlen( "Content-Type: text/html\n\n" );


	for(int i = 0; cgi_envp[i] != NULL; i++) {
		delete cgi_envp[i];
	}
	delete[] cgi_envp;
	close( fdpipe[0] );
	if(DEBUG_MODE)
		std::cout << "BUFF IS " << buff << std::endl;
	std::string out = request.get_httpver() + " 200 OK\n" + "Content-Length:" + std::to_string( len ) + "\n" + static_cast<std::string>(buff);
	send_response( out );
}

void ft::WebServer::response_GET( Request& request ) {

	//check if rights are correct
	//use index field from config somewhere here

	std::string response;
	if(is_directory( SERVER_DIR + request.get_requested_url() ) /*&& AUTOINDEX IS ON*/) {
		//list contents
		response = list_contents( SERVER_DIR + request.get_requested_url(), request);
	}
	else if(request.get_requested_url().find( "/cgi-bin/" ) == 0 && request.get_requested_url().size() > sizeof( "/cgi-bin/" )) { //if it is in /cgi-bin/
		execute_cgi( request );
		return;
	}
	else {
		//proceed with request
		std::string content_type = request.get_content_type();

		//read file to string
		std::ifstream infile( SERVER_DIR + request.get_requested_url() );
		if(!infile.is_open()) {
			handle_errors( 404, request);
		}
		std::string content( (std::istreambuf_iterator<char>( infile )),
			(std::istreambuf_iterator<char>()) );

		//first response line
		response = response + request.get_httpver() + " 200 OK\n";
		//write file to string
		response = response + "Content-Type: " + content_type + ";\nContent-Length:" + std::to_string( content.size() ) + "\n\n" + content;
	}
	//write string to socket
	send_response( response );
}

void ft::WebServer::response_DELETE( Request& request ) {
	//do smth
	// if(is_directory( SERVER_DIR + request.get_requested_url() ) /*&& AUTOINDEX IS ON*/) {
	// 	//list contents
	// 	response = list_contents( SERVER_DIR + request.get_requested_url() );
	// }
	std::ostringstream sresponse;
	if(std::remove( (SERVER_DIR + request.get_requested_url()).c_str() ) < 0) {
		//error
	}
	else {
		sresponse << "HTTP/1.1 200 OK\n\nFile " << request.get_requested_url() << " was successfully DELETED" << std::endl;
	}
	send_response( sresponse.str() );
}

void ft::WebServer::responder( Request& request ) {
	//check availability of method in location
	if(request.get_method() == GET)
		response_GET( request );
	else if(request.get_method() == POST)
		response_POST( request );
	else if(request.get_method() == DELETE)
		response_DELETE( request );
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
void ft::WebServer::handle_errors( int error_code, Request& request ) {
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
		"<link type=\"text/css\" rel=\"stylesheet\" href=\"/css/style.css\" />" << std::endl << std::endl << \
		"</head>" << std::endl << std::endl << \
		"<body>" << std::endl << \
		"<div class=\"vertical-center\">" << std::endl << \
		"<div class=\"container\">" << std::endl << \
		"<div id=\"notfound\" class=\"text-center\">" << std::endl << \
		"<h1>:-(</h1>"   <<std::endl << \
		"<h1>" << error_code << "</h1>" << std::endl << \
		"<p>" << response_messeges[error_code] << "</p>" << std::endl << \
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

	send_response( response );
	throw (error_request_code());
}
std::string ft::WebServer::list_contents( const std::string& path, Request& request )const {
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
	Request request;
	while(true) {
		std::cout << "waiting" << std::endl;
		try {
			request.clear();
			//poll will be here
			accepter(request);
			handler(request);
			responder(request);
			system( "leaks webserv" );
		}
		catch(error_request_code& e) {}
		std::cout << "==== DONE ====" << std::endl;
	}
}


ft::ListeningSocket* ft::WebServer::get_socket()const {
	return socket;
}


void ft::WebServer::send_response( const std::string& response ) const{
	write( new_socket, response.c_str(), response.size() );

	if(DEBUG_MODE)
		std::cout << GREEN << "===RESPONSE BEGIN===\n" << response << "\n===RESPONCE END===" << RESET << std::endl;

	close( new_socket );
}

std::string ft::WebServer::generate_response_head( const int& code ) {
	std::stringstream ss;
	ss << "HTTP/1.1 " << code << " " << response_messeges[code] << "\r\n";
	return ss.str();
}

void ft::WebServer::init_response_msgs() {
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
#include "WebServer.hpp"
#include <cstring>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstdio>
#include <fcntl.h>
#include <cctype>
#include <errno.h>
#include <cstdlib>
#include <map>
#include <signal.h>

ft::WebServer::WebServer( char** envp, ConfigInfo& config ) : envp( envp ), id(), config( config ), error_handler(Error_response_generator(&response_messeges)) { // зачем ID???
	std::vector<pollfd> fdset;
	for(int i = 0; i < config.getServers().size(); i++) {
		if(config.checkHostPortDublicates( i ) != NOT_FOUND)
			continue;
		if(DEBUG_MODE)
			std::cout << BLUE << "Listening host " << config.getServers()[i].getHost() << " with port " << config.getServers()[i].getListen() << RESET << std::endl;
		socket_array.push_back( ft::ListeningSocket( AF_INET, SOCK_STREAM, 0,
			config.getServers()[i].getListen(),
			config.getServers()[i].getHost(), BACKLOG ) );
		pollfd temp;
		temp.fd = get_socket_array().back().get_sock();
		temp.events = (POLLIN | POLLERR);
		temp.revents = 0;
		fdset.push_back( temp );
	}
	signal( SIGPIPE, SIG_IGN );//perhaps should elaborate more
	init_response_msgs();
	launch( fdset );
}

int ft::WebServer::accepter( int id ) {

	struct sockaddr_in address;
	address = socket_array[id].get_address();
	int addrlen = sizeof( sockaddr_in );

	int new_socket = accept( get_socket_array()[id].get_sock(), (struct sockaddr*)&address, (socklen_t*)&addrlen ); //
	fcntl( new_socket, F_SETFL, O_NONBLOCK );
	return new_socket;
}

bool ft::WebServer::response_POST( Request& request ) {
	// std::cout << "========RESPONSE POST IS ACTIVE========" << std::endl;

	if(request.get_requested_url().find( "/cgi-bin/" ) == 0 && request.get_requested_url().size() > 9 /*sizeof( "/cgi-bin/" )*/) { //if it is in /cgi-bin/
		return request.execute_cgi();
	}
	return true;
}

bool ft::WebServer::response_GET( Request& request ) {

	//check if rights are correct
	//use index field from config somewhere here

	if(is_directory( SERVER_DIR + request.get_requested_url() ) /*&& AUTOINDEX IS ON*/) {
		//list contents
		list_contents( SERVER_DIR + request.get_requested_url(), request );
		return true;
	}
	else if(is_directory( SERVER_DIR + request.get_requested_url() ) /*&& AUTOINDEX IS OFF*/) {
		handle_errors( 503, request );
		return true;
	}
	else if(request.get_requested_url().find( "/cgi-bin/" ) == 0 && request.get_requested_url().size() > sizeof( "/cgi-bin/" )) { //if it is in /cgi-bin/
		return request.execute_cgi();
	}
	else {
		//proceed with request
		std::string content_type = request.get_content_type();

		//read file to string
		std::ifstream infile( SERVER_DIR + request.get_requested_url() );
		if(!infile.is_open()) {
			handle_errors( 404, request );
			return true;
		}

		//first response line
		std::ofstream response_file;
		response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
		response_file << generate_response_head( 200 );
		//write file to string
		std::string* response_body = new std::string( (std::istreambuf_iterator<char>( infile )),
			(std::istreambuf_iterator<char>()) );
		response_file << "Content-Type: " << content_type << ";\nContent-Length:" << std::to_string( (*response_body).size() ) << "\n\n";
		response_file << *response_body;
		response_file.close();
		delete response_body;
	}
	return true;
	//write string to socket
}

bool ft::WebServer::response_DELETE( Request& request ) {
	//do smth
	// if(is_directory( SERVER_DIR + request.get_requested_url() ) /*&& AUTOINDEX IS ON*/) {
	// 	//list contents
	// 	response = list_contents( SERVER_DIR + request.get_requested_url() );
	// }
	std::ofstream response_file;
	if(std::remove( (SERVER_DIR + request.get_requested_url()).c_str() ) < 0) {
		//error
	}
	else {
		std::ofstream response_file;
		response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );

		std::stringstream msg;
		msg << "File " << request.get_requested_url() << " was successfully DELETED";
		response_file << generate_response_head( 200 ) << "Content-Type: text/html;\n" << "Content-Length: " << std::to_string( msg.str().size() ) << "\n\n" << msg.str();
		response_file.close();
	}
	return true;
}

bool ft::WebServer::generate_response( Request& request ) {
	//NYI check availability of method in location
	if(request.get_method() == GET)
		return response_GET( request );
	else if(request.get_method() == POST)
		return response_POST( request );
	else if(request.get_method() == DELETE)
		return response_DELETE( request );
	return true;
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
		else
		{
			//something else
			return 0;
		}
	}
	return 0;
}

void ft::WebServer::handle_errors(const int& error_code, Request& request ) {

	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
	if(!response_file.is_open()) {
		//press F to pay respects for the server
	}
	if(false /*custom errorpage exists*/) {
		// response_file << custom_error_page;
	}
	else {
		response_file << error_handler.generate_errorpage( error_code );
	}
	response_file.close();
	request.set_stage( RESPONCE_GENERATED );
}

// void ft::WebServer::handle_errors( int error_code, Request& request ) {
// 	std::ostringstream header;
// 	std::ostringstream body;

// 	body << "<!DOCTYPE html>" << std::endl << \
// 		"<html lang=\"en\">" << std::endl << std::endl << \
// 		"<head>" << std::endl << \
// 		"<title>" << error_code << " Error Page</title>" << std::endl << std::endl << \
// 		"<meta charset=\"utf-8\">" << std::endl << \
// 		"<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl << \
// 		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl << \
// 		"<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->" << std::endl << std::endl << \
// 		"<!-- Bootstrap -->" << std::endl << std::endl << \
// 		"<link rel = \"stylesheet\" href = \"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\">" << std::endl << std::endl << \
// 		"<!-- Custom stlylesheet -->" << std::endl << \
// 		"<link type=\"text/css\" rel=\"stylesheet\" href=\"/css/style.css\" />" << std::endl << std::endl << \
// 		"</head>" << std::endl << std::endl << \
// 		"<body>" << std::endl << \
// 		"<div class=\"vertical-center\">" << std::endl << \
// 		"<div class=\"container\">" << std::endl << \
// 		"<div id=\"notfound\" class=\"text-center\">" << std::endl << \
// 		"<h1>:-(</h1>" << std::endl << \
// 		"<h1>" << error_code << "</h1>" << std::endl << \
// 		"<p>" << response_messeges[error_code] << "</p>" << std::endl << \
// 		"<a href=\"/\">Back to homepage</a>" << std::endl << \
// 		"</div>" << std::endl << \
// 		"</div>" << std::endl << \
// 		"</div>" << std::endl << \
// 		"</body>" << std::endl << \
// 		"</html>" << std::endl;

// 	header << request.get_httpver() << " " << error_code << " " << "KO" << std::endl <<//<- needs elaboration
// 		"Content-Type: text/html;" << std::endl << \
// 		"Content-Length: " << body.str().size() << std::endl << std::endl;
// 	std::ofstream response_file;
// 	response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
// 	response_file << header.str() << body.str();
// 	response_file.close();
// 	request.set_stage( RESPONCE_GENERATED );

// 	// throw (error_request_code());
// }
void ft::WebServer::list_contents( const std::string& path, Request& request )const {
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

	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
	response_file << header.str() << body.str();
	response_file.close();
}

void ft::WebServer::launch( std::vector<pollfd>& fdset ) {

	if(DEBUG_MODE) {
		for(int i = 0; i < config.getServers().size(); i++) {
			std::cout << BOLDBLUE << (--config.getServers()[i].getLocations().end())->second << RESET << std::endl;;
		}
	}

	while(true) {
		if(DEBUG_MODE) {
			std::cout << "waiting" << std::endl;
		}
		newest_global_loop( fdset );
		if(DEBUG_MODE) {
			system( "leaks webserv" );
		}
		if(DEBUG_MODE) {
			std::cout << "==== DONE ====" << std::endl;
		}
	}
}

bool ft::WebServer::send_response( Request& request ) const {
	std::ifstream file;
	unsigned int needToReturn = 0;
	file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ) );
	if(!file.is_open()) {
		std::cout << RED << "send_response: " << strerror( errno ) << RESET << std::endl; // exeption
	}
	file.seekg( request.lastPos, std::ios_base::beg );
	char buffer[30000];
	file.read( buffer, 30000 );
	// check gcount
	unsigned int bytes_written = write( request.get_fd(), buffer, file.gcount() );
	if(bytes_written != file.gcount())
		needToReturn = file.gcount() - bytes_written;
	request.lastPos = file.tellg();
	request.lastPos -= needToReturn;
	if(file.eof()) {
		file.close();
		return true;
	}
	file.close();
	return false;
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

std::vector<ft::ListeningSocket> ft::WebServer::get_socket_array()const {
	return socket_array;
}

int ft::WebServer::get_size_serverInfo() const {
	return config.getServers().size();
}

void ft::WebServer::check_new_clients( std::vector<pollfd>& fdset, std::map<int, Request>& requests ) {
	for(int i = 0; i < get_size_serverInfo(); ++i) {
		if(fdset[i].revents & POLLIN) {
			pollfd temp;
			temp.fd = accepter( i );
			if(temp.fd == -1) //skip errors on accept
				continue;
			temp.events = (POLLIN | POLLERR);
			fdset.push_back( temp );
			requests[temp.fd] = Request();
			requests[temp.fd].set_fd( temp.fd );
			if(DEBUG_MODE) {
				std::cout << GREEN << i << ", on fd = " << fdset[i].fd << " request is accepted" << RESET << std::endl;

			}
		}
	}
}


void ft::WebServer::respond( pollfd& fdset, Request& request ) {
	int fdset_fd = fdset.fd;
	if(send_response( request )) {
		//this is highly questionable
		if(std::remove( (BUFFER_FILE + std::to_string( fdset_fd )).c_str() )) {
			//error
		}
		if(std::remove( (BUFFER_FILE_OUT + std::to_string( fdset_fd )).c_str() )) {
			//error
		}
		request = Request();
		request.set_fd(fdset_fd);
		fdset.events = (POLLIN | POLLERR);
		request.set_stage(REQUEST_PENDING);
	}
}


int ft::WebServer::recieve_request( pollfd& fdset, Request& request ) {
	request.set_request_handler();
	int handler_ret = request.execute_handler();
	request.set_cgi( envp );
	if(handler_ret == 1) { //returns if read is complete
		fdset.events = (POLLOUT | POLLERR);
	}
	else if(handler_ret == 2) {//returns if 0 bytes_read
		if(DEBUG_MODE)
			std::cout << GREEN << "read 0 on fd " << fdset.fd << RESET << std::endl;
		close( fdset.fd );
		if(std::remove( (BUFFER_FILE + std::to_string( fdset.fd )).c_str() )) {
			//error ignore?
		}
		if(std::remove( (BUFFER_FILE_OUT + std::to_string( fdset.fd )).c_str() )) {
			//error ignore?
		}
	}
	return handler_ret;
}

void ft::WebServer::work_with_clients( std::vector<pollfd>& fdset, std::map<int, Request>& requests ) {
	for(int i = fdset.size() - 1; i >= get_size_serverInfo(); --i) {
		pollfd& current_pollfd = fdset[i];
		Request& current_request = requests[current_pollfd.fd];
		
		if(current_pollfd.revents & POLLIN && current_request.is_pending()) {
			if(DEBUG_MODE) {
				std::cout << GREEN << i << ", fd = " << current_pollfd.fd << " is read" << RESET << std::endl;
			}
			int recieve_ret = recieve_request( current_pollfd, current_request );
			if(recieve_ret == 2) {//connection closed
				requests.erase( current_pollfd.fd );
				fdset.erase( fdset.begin() + i );
			}
			else if(recieve_ret == 1) {//we have read everything
				current_request.set_stage(REQUEST_FINISHED_READING);

			}
		}
		else if(current_request.is_finished_reading()) {
			if(generate_response( current_request )) {//response is ready
				current_request.set_stage(RESPONCE_GENERATED);
			}
		}
		else if(current_pollfd.revents & POLLOUT && current_request.responce_is_generated()) {
			if(DEBUG_MODE) {
				std::cout << GREEN << "socket " << i << ", fd = " << current_pollfd.fd << " is being written to" << RESET << std::endl;
			}
			respond( current_pollfd, current_request );
		}

	}
}


	void ft::WebServer::newest_global_loop( std::vector<pollfd>& fdset ) {
	std::map<int, Request> requests;
	bool is_cheking = true;
	while(true) {
		// for(int DEBUG_temp = 0; DEBUG_temp < 50; DEBUG_temp++) {
		int ret = poll( &fdset[0], fdset.size(), TIMEOUT );
		// for(int i = 0; i < fdset.size(); i++) {
		// 	std::cout << RED << i << " = " << fdset[i].fd << " | " << fdset[i].events << " | " << fdset[i].revents << RESET << std::endl;

		// }
		if(ret == 0) {
			//??????
			//close all and delete files????
		}
		if(ret == -1)
			std::cout << "Fail from poll\n";
		else if(ret > 0) {
			if(!is_cheking) {
				work_with_clients( fdset, requests );
				is_cheking = true;
			}
			if(is_cheking) {
				check_new_clients( fdset, requests );
				is_cheking = false;
			}
		}
	}
	exit( 1 );
}
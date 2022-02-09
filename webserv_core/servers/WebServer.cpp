#include "WebServer.hpp"
#include <cstring>
#include <string>
#include <sstream>
#include <dirent.h>
#include <sys/wait.h>
#include <cstdio>
#include <fcntl.h>
#include <cctype>
#include <errno.h>
#include <cstdlib>
#include <map>
#include <signal.h>
#include "../utils/utils.hpp"

ft::WebServer::WebServer( char** envp, ConfigInfo& config ) : envp( envp ), config( config ), error_handler( Error_response_generator( &response_messeges ) ), id() {
	std::vector<pollfd> fdset;
	fdset.reserve( 50000);
	for(std::size_t i = 0; i < config.getServers().size(); i++) {
		std::cout << MAGENTA << "Server http://" << config.getServers()[i].getHost() << ":" << config.getServers()[i].getListen() << " " <<config.getServers()[i].getServName() << RESET << std::endl;
		if(config.checkHostPortDublicates( i ) != NOT_FOUND)
			continue;
		if(DEBUG_MODE)
			std::cout << BLUE << "Listening host " << config.getServers()[i].getHost() << " with port " << config.getServers()[i].getListen() << RESET << std::endl;
		socket_array.push_back( ft::ListeningSocket( AF_INET, SOCK_STREAM, 0,
			config.getServers()[i].getListen(),
			config.getServers()[i].getHost(), BACKLOG ) );
		pollfd temp;
		temp.fd = get_socket_array().back().get_sock();
		temp.events = (POLLIN | POLLERR | POLLHUP | POLLNVAL);
		temp.revents = 0;
		fdset.push_back( temp );
	}
	signal( SIGPIPE, SIG_IGN );
	init_response_msgs();
	launch( fdset );
}

int ft::WebServer::accept_connection( const std::size_t& id ) {

	struct sockaddr_in address;
	address = socket_array[id].get_address();
	int addrlen = sizeof( sockaddr_in );

	int new_socket = accept( get_socket_array()[id].get_sock(), (struct sockaddr*)&address, (socklen_t*)&addrlen ); //
	fcntl( new_socket, F_SETFL, O_NONBLOCK );
	return new_socket;
}

void ft::WebServer::write_response_to_file( Request& request ) {

	std::ifstream infile( request.get_rooted_url() );
	if(!infile.is_open()) {
		handle_errors( 404, request );
		return;
	}
	infile.seekg( 0, std::ios::end );
	long requested_file_size = infile.tellg();
	infile.seekg( 0, std::ios::beg );

	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
	response_file << generate_response_head( 200, request);
	response_file << "Content-Type: " << request.get_content_type() << ";\nContent-Length:" \
		<< std::to_string( requested_file_size ) << "\n\n";

	char buffer[CGI_BUFFER_SIZE + 1];
	while(!infile.eof()) {
		bzero( buffer, CGI_BUFFER_SIZE );
		infile.read( buffer, CGI_BUFFER_SIZE );
		response_file.write( buffer, infile.gcount() );
	}
	infile.close();
	response_file.close();
}

void ft::WebServer::generate_upload_response( Request& request) {
	std::ostringstream header;
	std::ostringstream body;

	std::size_t last_slash = request.get_requested_url().find_last_of( "/" );
	if(last_slash == 0) {
		last_slash = 1;
	}
	std::string old_url = request.get_requested_url().substr( 0, last_slash );
	
	std::string path = config.getRootedUrl( request.get_servID(), old_url ) + \
		config.getUploadPath( request.get_servID(), request.get_requested_url() ) + "/";
	body << "<!DOCTYPE html><html><body><form id = \"uploadbanner\" enctype = \"multipart/form-data\" method = \"post\" action = \"/cgi-bin/upload";
	body << "?path=" << path << "\"><input id = \"fileupload\" name = \"myfile\" type = \"file\"/><input type = \"submit\" value = \"submit\" id = \"submit\"/></form></body></html>";

	header << generate_response_head( 200, request );
	header << "Content-Type: text/html;" << std::endl << \
		"Content-Length: " << body.str().size() << std::endl << std::endl;

	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
	response_file << header.str() << body.str();
	response_file.close();
}

bool ft::WebServer::response_GET_POST( Request& request ) {
	if(is_directory( request.get_rooted_url())) {
		if(config.getAutoIndex( request.get_servID(), request.get_requested_url()/*AUTOINDEX IS ON*/ )) {
			list_contents( request.get_rooted_url() , request );
			return true;
		}
		else if("" != config.getIndex( request.get_servID(), request.get_requested_url() )) {
			request.set_requested_url( request.get_requested_url() + config.getIndex( request.get_servID(), request.get_requested_url() ) );
			request.set_rooted_url( config.getRootedUrl( request.get_servID(), request.get_requested_url() ) );
		}
		else {
			handle_errors( 403, request );
			return true;
		}
	}
	if(request.get_requested_filename() == "/upload.html") {
		generate_upload_response(request);
		return true;
	}
	if(request.get_requested_url().find( "/cgi-bin/" ) != std::string::npos \
		&& request.get_requested_url().find( "/cgi-bin/" ) == 0 && \
		request.get_requested_url().size() > sizeof( "/cgi-bin/" )) { //if it is in /cgi-bin/
		int exit_code = request.execute_cgi();
		if(exit_code && exit_code != 1) {
			if(response_messeges.find( exit_code ) == response_messeges.end()) {
				exit_code = 500;
			}
			handle_errors( exit_code, request );
			return true;
		}
		return static_cast<bool>(request.execute_cgi());
	}
	else {
		write_response_to_file( request );
	}
	return true;
}

bool ft::WebServer::response_DELETE( Request& request ) {
	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
	std::stringstream msg;
	if(is_directory( request.get_rooted_url() )) {
		msg << "Your " << request.get_requested_url() << " is a directory, denied";
		response_file << generate_response_head( 403, request ) << "Content-Type: text/html;\n" << \
			"Content-Length: " << std::to_string( msg.str().size() ) << "\n\n" << msg.str();
		return true;
	}
	if(std::remove( (request.get_rooted_url()).c_str() ) < 0) {
		msg << "File " << request.get_rooted_url() << " was not deleted, error occured";
		response_file << generate_response_head( 200, request ) << "Content-Type: text/html;\n" << "Content-Length: " << std::to_string( msg.str().size() ) << "\n\n" << msg.str();
	}
	else {
		msg << "File " << request.get_rooted_url() << " was successfully DELETED";
		response_file << generate_response_head( 200, request ) << "Content-Type: text/html;\n" << "Content-Length: " << std::to_string( msg.str().size() ) << "\n\n" << msg.str();
	}
	response_file.close();
	return true;
}

bool ft::WebServer::generate_response( Request& request ) {
	int method = request.get_method();
	if(method == GET)
		return response_GET_POST( request );
	else if(method == POST)
		return response_GET_POST( request );
	else if(method == DELETE)
		return response_DELETE( request );
	return true;
}


void ft::WebServer::hard_close_connection( Request& request ) {
	request.set_stage( RESPONCE_GENERATED );
	request.set_fd_events( POLLOUT | POLLERR | POLLHUP | POLLNVAL );
}

void ft::WebServer::handle_errors( const int& error_code, Request& request ) {

	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
	if(!response_file.is_open()) {
		//press F to pay respects for the server, emergency, no way around
	}
	std::string error_page = "";
	if(error_code != 421) {
		std::string loc = config.getLocationByID( request.get_servID(), request.get_requested_url() );
		if(config.getErrorPage( request.get_servID(), loc, error_code ) != "") {
			error_page = config.getRootedUrl( request.get_servID(), loc );
			error_page += config.getErrorPage( request.get_servID(), loc, error_code );
		}
	}
	if(DEBUG_MODE)
		std::cout << BOLDRED << error_code << " " << error_page << RESET << std::endl;
	if(error_code == 413) {
		request.cease_after_msg = true;
	}
	if(error_page != "") {
		int file_size = get_file_size( error_page );
		std::ifstream error_page_file( error_page );
		char buffer[30000];
		if(error_page_file.is_open()) {
			response_file << "HTTP/1.1" << " " << error_code << " " << response_messeges.at( error_code ) << "\n";
			if(request.cease_after_msg) {
				response_file << "Connection: close;" << std::endl;
			}
			response_file << "Content-Type: text/html;" << std::endl << \
				"Content-Length: " << std::to_string( file_size ) << "\r\n\r\n";
			while(!error_page_file.eof()) {
				error_page_file.read( buffer, 30000 );
				response_file.write( buffer, error_page_file.gcount() );
			}
		}
		else
			response_file << error_handler.generate_errorpage( error_code, request.get_cookie(), request.cease_after_msg );
	}
	else
		response_file << error_handler.generate_errorpage( error_code, request.get_cookie(), request.cease_after_msg );
	response_file.close();
	request.set_stage( RESPONCE_GENERATED );
	request.set_fd_events( POLLOUT | POLLERR | POLLHUP | POLLNVAL );
}

void ft::WebServer::list_contents( const std::string& path, Request& request ) {
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
		"<body style=\"background-color:" << request.get_cookie() << ";\">" << std::endl << \
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
	//buttons for background color
	body << "<br><br><br><br><br><br><br><br><br>";
    body << "<div style=\"display:flex\">";
	body << "<form method=\"GET\" action=\"" << request.get_requested_url() << "\">" << \
		"<input type=\"hidden\" name=\"color\" value=\"Lavender\">"<< \
		"<button style=\"background-color:Lavender;height:50px;width:50px;\" type=\"submit\"></button></form>";
	body << "<form method=\"GET\" action=\"" << request.get_requested_url() << "\">"  \
         << "<input type=\"hidden\" name=\"color\" value=\"HoneyDew\">" \
        << "<button style=\"background-color:HoneyDew;height:50px;width:50px; type=\"submit\"></button></form>";

    body << "<form method=\"GET\" action=\"" << request.get_requested_url() << "\">" << \
		"<input type=\"hidden\" name=\"color\" value=\"LightCyan\">" << \
		"<button style=\"background-color:LightCyan;height:50px;width:50px;\" type=\"submit\"></button></form>";
    body << "<form method=\"GET\" action=\"" << request.get_requested_url() << "\">" << \
		"<input type=\"hidden\" name=\"color\" value=\"Cornsilk\">" << \
		"<button style=\"background-color:Cornsilk;height:50px;width:50px;\" type=\"submit\"></button></form>";
    body << "<form method=\"GET\" action=\"" << request.get_requested_url() << "\">" << \
		"<input type=\"hidden\" name=\"color\" value=\"PaleGreen\">" << \
		"<button style=\"background-color:PaleGreen;height:50px;width:50px;\" type=\"submit\"></button></form>";
	
	body << "<form method=\"GET\" action=\"" << request.get_requested_url() << "\">" << \
		"<input type=\"hidden\" name=\"color\" value=\"Pink\">" << \
		"<button style=\"background-color:Pink;height:50px;width:50px;\" type=\"submit\"></button></form>";
	body << "</div>";

	//button for upload
	body << "<br><br>";
	body << "<a href=\"" << request.get_requested_url();
	if(request.get_requested_url()[request.get_requested_url().size() - 1] != '/')
		body << "/";
	body << "upload.html\">Upload file</a><br>";
	
	body << "</body>" << std::endl << \
		"</html>" << std::endl;

	header << generate_response_head( 200, request ) << \
		"Content-Type: text/html;" << std::endl << \
		"Content-Length: " << body.str().size() << std::endl << std::endl;

	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
	response_file << header.str() << body.str();
	response_file.close();
}

void ft::WebServer::launch( std::vector<pollfd>& fdset ) {

	if(DEBUG_MODE) {
		for(std::size_t i = 0; i < config.getServers().size(); i++) {
			std::cout << BOLDBLUE << (--config.getServers()[i].getLocations().end())->second << RESET << std::endl;;
		}
	}

	while(true) {
		if(DEBUG_MODE) {
			std::cout << "waiting" << std::endl;
		}
		try {
			global_loop( fdset );
		}
		catch(std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
		if(DEBUG_MODE) {
			std::cout << "==== DONE ====" << std::endl;
		}
	}
}

int ft::WebServer::send_response( Request& request ) const {
	std::ifstream file;
	unsigned int needToReturn = 0;
	file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ) );
	if(!file.is_open()) {
		return 2;
	}
	file.seekg( request.lastPos, std::ios_base::beg );
	char buffer[30000];
	file.read( buffer, 30000 );
	unsigned int bytes_written = write( request.get_fd(), buffer, file.gcount() );
	if(bytes_written < 1) {
		return 2;
	}
	
	if(bytes_written != file.gcount())
		needToReturn = file.gcount() - bytes_written;
	request.lastPos = file.tellg();
	request.lastPos -= needToReturn;
	if(file.eof()) {
		file.close();
		return 1;
	}
	file.close();
	return 0;
}

std::string ft::WebServer::generate_response_head( const int& code, Request& request){
	std::stringstream ss;
	ss << "HTTP/1.1 " << code << " " << response_messeges[code] << "\r\n";
	std::string cookie = request.get_cookie();
	if(request.get_param_value( "HTTP_COOKIE" ) != "color=" + cookie)
		ss << "Set-Cookie:color=" << cookie << "\n";
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
	response_messeges[418] = "I am a teapot :-)";
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
	for(std::size_t i = 0; i < socket_array.size(); ++i) {
		if(fdset[i].revents & POLLIN) {
			pollfd temp;
			temp.fd = accept_connection( i );
			if(temp.fd == -1) //skip errors on accept
				continue;
			temp.events = (POLLIN | POLLERR | POLLHUP | POLLNVAL);
			fdset.push_back( temp );
			requests[temp.fd] = Request();
			requests[temp.fd].set_socket( &socket_array[i] );
			if(DEBUG_MODE)
				std::cout << RED << socket_array[i].get_ip() << " " << socket_array[i].get_port() << RESET << std::endl;
			// requests[temp.fd].set_fd( temp.fd );
			requests[temp.fd].set_fdset( &fdset[fdset.size() - 1] );
			if(DEBUG_MODE) {
				std::cout << GREEN << i << ", on fd = " << fdset[i].fd << " request is accepted" << RESET << std::endl;

			}
		}
	}
}

int ft::WebServer::remove_buffer_files( const int& fdset_fd ) {
	if(std::remove( (BUFFER_FILE + std::to_string( fdset_fd )).c_str() )) {
		return 1;
	}
	if(std::remove( (BUFFER_FILE_OUT + std::to_string( fdset_fd )).c_str() )) {
		return 1;
	}
	return 0;
}

bool ft::WebServer::respond( pollfd& fdset, Request& request ) {
	int fdset_fd = fdset.fd;
	int ret = send_response( request );
	if(ret == 1) {
		if(remove_buffer_files( fdset_fd )) {
			//error deleting, this is fine
		}
		return true;
	}
	else if(ret == 2) {
		request.cease_after_msg = true;
		hard_close_connection( request );
		return true;
	}
	return false;
}


int ft::WebServer::get_serverID( Request& request ) {
	return config.getServerID( request.get_serverIP(), request.get_serverPort(), request.get_serverName() );
}

void ft::WebServer::generate_redirect_response( const int& code, Request& request, const std::string& redirect_url ) {
	std::ofstream response_file;
	response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
	if(DEBUG_MODE)
		std::cout << GREEN << "YOU ARE BEING REDIRECTED" << RESET << std::endl;
	response_file << generate_response_head( code, request );
	if(code != 302)
		response_file << "Location: " << redirect_url;
	response_file << "\r\n\r\n";
	response_file.close();
	request.cease_after_msg = true;
	request.set_stage( RESPONCE_GENERATED );
	request.set_fd_events( POLLOUT | POLLERR | POLLHUP | POLLNVAL );
}

bool ft::WebServer::respond_out_of_line( Request& request, pollfd& fdset ) {
	int serverID = request.get_servID();
	std::string requested_url = request.get_requested_url();
	bool is_chunked = request.is_chunked();
	int body_size = 0;
	
	if(serverID == -2) {//happends once per request
		request.set_servID( get_serverID( request ) );
		serverID = request.get_servID();
		if(serverID == -1) {
			//ERRORresolved no server with such servername
			handle_errors( 421, request );
			return true;
		}
		int return_code = 0;
		
		std::string redirect_url = config.getRedirect( serverID, requested_url, return_code );

		if(DEBUG_MODE)
			std::cout << GREEN << "servid =" << serverID << " url = " << requested_url << " returned_code = " << return_code << " returnd url = " << redirect_url << RESET << std::endl;
		if(return_code != 0) {
			generate_redirect_response( return_code, request, redirect_url );
			return true;
		}
		if(!config.checkMethod( serverID, requested_url, static_cast<method>(request.get_method()) )) {
			//ERRORresolved
			if(request.get_method() == POST) {
				//clear not read body from socket
				request.cease_after_msg = true;
			}
			handle_errors( 405, request );//method forbidden
			return true;
		}
		body_size = config.getBodySize( serverID, requested_url );
		request.set_rooted_url( config.getRootedUrl( serverID, requested_url ) );
		if(!is_chunked && body_size && strtol( request.get_param_value( "HTTP_CONTENT_LENGTH" ).c_str(), NULL, 10 ) > body_size) {
			//ERRORresolved bodysize too big
			handle_errors(413, request );
			std::cout << body_size << std::endl;
			return true;
		}
	}
	body_size = config.getBodySize( serverID, requested_url );
	if(is_chunked && body_size && get_file_size( BUFFER_FILE + std::to_string( fdset.fd ) ) > body_size) {
		//ERRORresolved bodysize too big, chunked requests
		handle_errors( 413, request );
		return true;
	}
	return false;
}


int ft::WebServer::recieve_request( pollfd& fdset, Request& request ) {
	request.set_request_handler();
	int handler_ret = request.execute_handler();

	if(handler_ret == -2) {
		request.cease_after_msg = true;
		hard_close_connection( request );
		return -1;
	}
	if(handler_ret > 2) {
		handle_errors( handler_ret, request );
		return -1;
	}
	else if(handler_ret == 2) {//returns if 0 bytes_read
		if(DEBUG_MODE)
			std::cout << GREEN << "read 0 on fd " << fdset.fd << RESET << std::endl;
		close( fdset.fd );
		if(remove_buffer_files( fdset.fd )) {
			//error deleting, this is fine
		}
		return 2;
	}
	else if(handler_ret == -1) {
		handle_errors( 431, request );
		return -1;
		//ERRORresolved returns if headers are too long
	}
	if(request.get_param_value( "HTTP_COOKIE" ) != "") {
		request.set_cookie( request.get_param_value( "HTTP_COOKIE" ) );
	}
	if(request.get_query_string() != "")
		request.set_cookie( request.get_query_string() );

	if(DEBUG_MODE){
		std::cout << MAGENTA << request.get_requested_url() << RESET << std::endl;
		request.print_params();
	}
	if(respond_out_of_line( request, fdset )) {
		return -1;
	}
	request.set_cgi( envp, config.getCGI( request.get_servID(), ".py" ), config.getCGI( request.get_servID(), ".pl" ) );
	if(handler_ret == 1) { //returns if read is complete
		fdset.events = (POLLOUT | POLLERR | POLLHUP | POLLNVAL);
	}
	return handler_ret;
}


void ft::WebServer::remove_hungup( std::vector<pollfd>& fdset, std::map<int, Request>& requests, const int& i) {
	pollfd& current_pollfd = fdset[i];
	
	remove_buffer_files( current_pollfd.fd );
	close( current_pollfd.fd );
	requests.erase( current_pollfd.fd );
	fdset.erase( fdset.begin() + i );
}


void ft::WebServer::recieve_avaliable( std::vector<pollfd>& fdset, std::map<int, Request>& requests, const int& i ) {
	pollfd& current_pollfd = fdset[i];
	Request& current_request = requests[current_pollfd.fd];
	if(DEBUG_MODE) {
		std::cout << GREEN << i << ", fd = " << current_pollfd.fd << " is read" << RESET << std::endl;
	}
	int recieve_ret = recieve_request( current_pollfd, current_request );
	if(recieve_ret == 2) {//connection closed
		close( current_pollfd.fd );
		remove_buffer_files( current_pollfd.fd );
		requests.erase( current_pollfd.fd );
		fdset.erase( fdset.begin() + i );
	}
	else if(recieve_ret == 1) {//we have read everything
		current_request.set_stage( REQUEST_FINISHED_READING );
	}
	else if(recieve_ret == -1) {
		//do nothing, everything is already set in place
	}
}


void ft::WebServer::respond_avaliable( std::vector<pollfd>& fdset, std::map<int, Request>& requests, const int& i ) {
	pollfd& current_pollfd = fdset[i];
	Request& current_request = requests[current_pollfd.fd];
	if(DEBUG_MODE) {
		std::cout << GREEN << "socket " << i << ", fd = " << current_pollfd.fd << " is being written to" << RESET << std::endl;
	}
	if(respond( current_pollfd, current_request )) {
		if(current_request.cease_after_msg) {
			close( current_pollfd.fd );
			remove_buffer_files( current_pollfd.fd );
			requests.erase( current_pollfd.fd );
			fdset.erase( fdset.begin() + i );
		}
		else {
			reset_request( current_pollfd, current_request );
		}
	}
}
void ft::WebServer::work_with_clients( std::vector<pollfd>& fdset, std::map<int, Request>& requests ) {
	for(std::size_t i = fdset.size() - 1; i >= socket_array.size(); --i) {
		pollfd& current_pollfd = fdset[i];
		Request& current_request = requests[current_pollfd.fd];
		
		if(current_pollfd.revents & POLLHUP || current_pollfd.revents & POLLERR || \
			current_pollfd.revents & POLLNVAL) {
			remove_hungup( fdset, requests, i );
		}
		else if(current_pollfd.revents & POLLIN && current_request.is_pending()) {
			recieve_avaliable( fdset, requests, i );
		}
		else if(current_request.is_finished_reading()) {
			if(generate_response( current_request )) {//response is ready
				current_request.set_stage(RESPONCE_GENERATED);
			}
		}
		else if(current_pollfd.revents & POLLOUT && current_request.responce_is_generated()) {
			respond_avaliable( fdset, requests, i );
		}
	}
}


void ft::WebServer::reset_request( pollfd& fdset, Request& request ) {
	const ListeningSocket* temp_sock = request.get_socket();
	request = Request();

	request.set_fdset( &fdset );
	if(DEBUG_MODE)
		std::cout << BLUE << "FD = " << request.get_fd() << " is reset" << RESET << std::endl;
	request.set_socket( temp_sock );
	fdset.events = (POLLIN | POLLERR | POLLNVAL | POLLHUP);
	request.set_stage( REQUEST_PENDING );
}

void ft::WebServer::global_loop( std::vector<pollfd>& fdset ) {
	std::map<int, Request> requests;
	bool is_cheking = true;
	while(true) {
		// system( "leaks webserv" );
		// for(std::size_t i = 0; i < fdset.size(); i++) {
		// 	std::cout << RED << i << " = " << fdset[i].fd << " | " << fdset[i].events << " | " << fdset[i].revents << RESET << std::endl;
		// }
		// std::cout << BOLDRED << "POLL" << RESET << std::endl;
		int ret = poll( &fdset[0], fdset.size(), TIMEOUT );
		if(ret == 0) {}
		if(ret == -1)
			throw (std::exception());
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
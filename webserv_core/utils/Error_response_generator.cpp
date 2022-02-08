#include <map>
#include<string>
#include "Error_response_generator.hpp"
#include <sstream>

namespace ft {
	Error_response_generator::Error_response_generator( const std::map<int, std::string>* response_msgs ) : response_messeges( response_msgs){
		
	}

	Error_response_generator::~Error_response_generator(){}
	
	std::string Error_response_generator::generate_errorpage( const int& error_code, const std::string& color,const bool& cease_connection) const {
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
			"<body style=\"background-color:" << color << ";\">" << std::endl << \
			"<div class=\"vertical-center\">" << std::endl << \
			"<div class=\"container\">" << std::endl << \
			"<div id=\"notfound\" class=\"text-center\">" << std::endl << \
			"<h1>:-(</h1>" << std::endl << \
			"<h1>" << error_code << "</h1>" << std::endl << \
			"<p>" << response_messeges->at(error_code) << "</p>" << std::endl << \
			"<a href=\"/\">Back to homepage</a>" << std::endl << \
			"</div>" << std::endl << \
			"</div>" << std::endl << \
			"</div>" << std::endl << \
			"</body>" << std::endl << \
			"</html>" << std::endl;


		header << "HTTP/1.1" << " " << error_code << " " << response_messeges->at( error_code ) << "\n";
		if(cease_connection) {
			header << "Connection: close;" << std::endl;
		}
		header << "Content-Type: text/html;" << std::endl << \
			"Content-Length: " << body.str().size() << std::endl << std::endl;

		std::ostringstream out;
		out << header.str() << body.str();
		return out.str();
		
		// std::ofstream response_file;
		// response_file.open( BUFFER_FILE_OUT + std::to_string( request.get_fd() ), std::ios::binary );
		// response_file << header.str() << body.str();
		// response_file.close();
		// request.set_stage( RESPONCE_GENERATED );

		// throw (error_request_code());
	}
}
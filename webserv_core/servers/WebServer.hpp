#include <string>
#include "../utils/Request.hpp"
#include <exception>
#include "../resources/defines.hpp"
#include "../config_parser/Config_info.hpp"
#include "../sockets/ListeningSocket.hpp"
namespace ft {
	class WebServer {
	private:
		class error_request_code : public std::exception {
			virtual const char* what() const throw();
		};
		//thing to change
		ListeningSocket* socket;

		std::vector<ServerConfig> config;		
		int new_socket;
		char** envp;
		
	public:
		WebServer(char **envp, Config_info &config);
		void launch();
	private:
		void accepter( Request& );
		void handler( Request& );
		void responder( Request&);

		bool is_directory( const std::string& path )const;
		std::string list_contents( const std::string& path, Request& request )const;
		void handle_errors( int error_code, Request& request );
		void response_POST( Request& request );
		void response_GET(Request& request);
		void response_DELETE( Request& request );
		void header_parse( std::string&, Request& );
		char** create_appended_envp( Request& request );
		
		ListeningSocket* get_socket()const;
	};
}
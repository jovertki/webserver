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
		Request request;
	public:
		WebServer(char **envp, Config_info &config);
		void launch();
	private:
		void accepter();
		void handler();
		void responder();

		bool is_directory( const std::string& path )const;
		std::string list_contents( const std::string& path )const;
		void handle_errors( int error_code );
		void response_POST();
		void response_GET();
		void response_DELETE();
		void header_parse( std::string& );

		ListeningSocket* get_socket()const;
	};
}
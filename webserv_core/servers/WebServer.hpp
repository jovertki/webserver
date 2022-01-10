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

		Config_info config;
		int new_socket;
		char** envp;

		std::map<int, std::string> response_messeges;
	public:
		WebServer(char **envp, Config_info &config);
		void launch();
	private:
		void accepter( Request& );
		void handler( Request& );
		void responder( Request&);

		bool is_directory( const std::string& path )const;
		std::string* list_contents( const std::string& path, Request& request )const;
		void handle_errors( int error_code, Request& request );
		void response_POST( Request& request );
		void response_GET(Request& request);
		void response_DELETE( Request& request );
		void execute_cgi( Request& request );
		void header_parse( const char*, Request& );
		char** create_appended_envp( Request& request );
		void init_new_envp( std::map<std::string, std::string>&, Request& );
		void send_response( const std::string& response )const;
		void send_response( const std::string& response, const std::string* content ) const;
		void send_response( const std::string* response ) const;
		void init_response_msgs();
		void handle_multipart( Request& request, \
			char* buffer, long& bytes_read, std::ofstream& body_file, \
			long& total_bytes_read, long& full_request_length, \
			bool& parsing_data_header, bool header_included );

		std::string generate_response_head( const int& code );
		ListeningSocket* get_socket()const;
	};
}
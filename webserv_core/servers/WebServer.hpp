#include <string>
#include "../utils/Request.hpp"
#include <exception>
#include "../resources/defines.hpp"
#include "../config_parser/Config_info.hpp"
#include "../sockets/ListeningSocket.hpp"
#include <sys/poll.h> //

namespace ft {
	class WebServer {
	private:
		class error_request_code : public std::exception {
			virtual const char* what() const throw();
		};
		//thing to change
		// ListeningSocket* socket;
		std::vector<ListeningSocket> socket_array; //
		// pollfd fdset[get_size_serverInfo()];

		std::vector<ServerConfig> serverInfo;
		Config_info config;
		int new_socket;
		char** envp;

		std::map<int, std::string> response_messeges;
	public:
		WebServer(char **envp, Config_info &config);
		void launch(struct pollfd fdset[]);
		int id;
		
	private:
		void poller(struct pollfd fdset[]); //
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
		// void send_response( const std::string& response )const;
		// void send_response( const std::string& response, const std::string* content ) const;
		// void send_response( const std::string* response ) const;
		void send_response( const std::fstream& response_file, const long& response_length ) const;
		void init_response_msgs();
		void handle_multipart( Request& request, \
			char* buffer, long& bytes_read, std::ofstream& body_file);




		void new_global_loop( struct pollfd fdset[] );
		std::vector<ListeningSocket> get_socket_array()const;
		int get_size_serverInfo() const;
		std::string generate_response_head( const int& code );
		void new_handler( Request& );
		// ListeningSocket* get_socket()const;
	};
}
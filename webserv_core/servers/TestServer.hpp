#include "SimpleServer.hpp"
#include <string>
#include "../utils/Request.hpp"
#include <exception>
#include "../resources/defines.hpp"
namespace ft {
	class TestServer : public SimpleServer {
	private:
		class error_request_code : public std::exception {
			virtual const char* what() const throw();
		};
		char** envp;
		char buffer[30001] = {0};
		int new_socket;
		std::string buffer_s;
		Request request;
		void accepter();
		void handler();
		void responder();

		bool is_directory( const std::string& path )const;
		std::string list_contents( const std::string& path )const;
		void handle_errors( int error_code );
		void response_POST();
		void response_GET();
		void response_DELETE();
	public:
		TestServer(char **envp);
		void launch();
	};
}
#include "SimpleServer.hpp"
#include <string>
#include "../utils/Request.hpp"
namespace ft {	
	class TestServer : public SimpleServer {
	private:
		char** envp;
		char buffer[30000] = {0};
		int new_socket;
		std::string buffer_s;
		Request request;
		void accepter();
		void handler();
		void responder();
		
		void response_post();
		void response_get();
	public:
		TestServer(char **envp);
		void launch();
	};
}
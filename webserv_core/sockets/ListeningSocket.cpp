#include "ListeningSocket.hpp"

ft::ListeningSocket::ListeningSocket( int domain, int service, int protocol, int port, std::string interface, int bklg ) : \
BindingSocket( domain, service, protocol, port, interface ) {
	backlog = bklg;
	start_listening();
	std::cout << "listening" << std::endl;
	test_connection( listening );
}


void ft::ListeningSocket::start_listening() {
	listening = listen( get_sock(), backlog );
}


int ft::ListeningSocket::get_listening() {
	return listening;
}

int ft::ListeningSocket::get_backlog() {
	return backlog;
}
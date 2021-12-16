#include "ListeningSocket.hpp"

ft::ListeningSocket::ListeningSocket( int domain, int service, int protocol, int port, u_long interface, int bklg ) : \
BindingSocket( domain, service, protocol, port, interface ) {
	backlog = bklg;
	start_listening();
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
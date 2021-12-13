
#include "SimpleServer.hpp"

ft::SimpleServer::SimpleServer( int domain, int service, int protocol, int port, u_long interface, int bklg ) {
	socket = new ft::ListeningSocket( domain, service, protocol, port, interface, bklg );
}



ft::ListeningSocket* ft::SimpleServer::get_socket() {
	return socket;
}


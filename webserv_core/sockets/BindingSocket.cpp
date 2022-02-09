#include "BindingSocket.hpp"
#include <fcntl.h>
ft::BindingSocket::BindingSocket( int domain, int service, int protocol, int port, std::string interface ) : \
SimpleSocket( domain, service, protocol, port, interface ) {
	// std::cout << "binding" << std::endl;
	test_connection( connect_to_network( get_sock(), get_address() ) );
}


int ft::BindingSocket::connect_to_network( int sock, struct sockaddr_in address ) {
	fcntl(sock, F_SETFL , O_NONBLOCK);
	return bind( sock, (struct sockaddr*)&address, sizeof( address ) );
}
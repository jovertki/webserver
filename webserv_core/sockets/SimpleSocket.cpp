
#include "SimpleSocket.hpp"

ft::SimpleSocket::SimpleSocket( int domain, int service, int protocol, int port, u_long interface ){
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(interface);
	//establish socket
	sock = socket( domain, service, protocol );

	
	int yes = 1;//no idea how this works
	setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( yes ) );
	//confirm the connection to be successful
	test_connection(sock);
}

void ft::SimpleSocket::test_connection( int item_to_test){
	if (item_to_test < 0){
		perror("Failed to connect");
		exit(EXIT_FAILURE);
	}
}

int ft::SimpleSocket::get_sock(){
	return sock;
}
struct sockaddr_in ft::SimpleSocket::get_address(){
	return address;
}
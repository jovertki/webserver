#pragma once
#include <cstddef>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
namespace ft {
	class SimpleSocket {
	private:
		int sock;
		struct sockaddr_in address;
	public:
		SimpleSocket( int domain, int service, int protocol, int port, std::string interface );
		virtual ~SimpleSocket() {}
		virtual int connect_to_network( int sock, struct sockaddr_in address ) = 0;
		void test_connection( int );

		int get_sock();
		struct sockaddr_in get_address();
	};

}
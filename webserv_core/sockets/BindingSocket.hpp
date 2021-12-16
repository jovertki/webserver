#include "SimpleSocket.hpp"


namespace ft {
	class BindingSocket : public SimpleSocket {
	private:
		int binding;
		int connect_to_network( int sock, struct sockaddr_in address );
	public:
		BindingSocket( int domain, int service, int protocol, int port, u_long interface );

		int get_binding();
	};
}
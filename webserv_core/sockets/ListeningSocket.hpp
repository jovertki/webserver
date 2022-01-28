#pragma once
#include "BindingSocket.hpp"

namespace ft{
	class ListeningSocket : public BindingSocket{
	private:
		int backlog;
		int listening;
	public:
		ListeningSocket( int domain, int service, int protocol, int port, std::string interface, int bklg );
		void start_listening();

		int get_listening();
		int get_backlog();
	};
}
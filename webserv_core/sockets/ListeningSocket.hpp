#pragma once
#include "BindingSocket.hpp"

namespace ft{
	class ListeningSocket : public BindingSocket{
	private:
		int backlog;
		int listening;

		int port;
		std::string ip;
	public:
		ListeningSocket( int domain, int service, int protocol, int port, std::string interface, int bklg );
		virtual ~ListeningSocket() {}
		void start_listening();


		int get_port()const;
		std::string get_ip()const;
		int get_listening();
		int get_backlog();
	};
}
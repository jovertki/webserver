#pragma once
#include "../resources/defines.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <string>

namespace ft {
	class CGI_handler {
	private:
		int stage;
		pid_t cgi_pid;
		char** envp;
		std::map<int, std::string> response_messeges;

		char** create_appended_envp();
		void init_new_envp( std::map<std::string, std::string>& );
		void start();
		void process();
		void write();
		void init_response_msgs();
		std::string generate_response_head( const int& code );
		// CGI_handler& operator=(const CGI_handler& a);
	public:
		CGI_handler( char** envp = NULL, const int& afd = 0, \
		const std::string& arequested_url = "", const std::string& aquery_string = "", \
		const int& amethod = 0, const std::map <std::string, std::string>& aparams = \
		std::map <std::string, std::string>());

		CGI_handler(const CGI_handler& a);
		~CGI_handler();
		
		//request stuff
		int fd;
		std::string requested_url;
		std::string query_string;
		int method;

		std::map <std::string, std::string> params;
		bool execute();
	};
}
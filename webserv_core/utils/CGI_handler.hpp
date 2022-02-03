#pragma once
#include "../resources/defines.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <string>

namespace ft {
	class CGI_handler {
	public:
		CGI_handler( char** envp = NULL,  int* afd = NULL, \
			std::string* arequested_url = NULL,  std::string* aquery_string = NULL, \
			int* amethod = NULL,  std::map <std::string, std::string>* aparams = NULL );

		CGI_handler(const CGI_handler& a);
		~CGI_handler();
		
		bool is_initialised();
		bool execute();
	private:
		int stage;
		pid_t cgi_pid;
		char** envp;
		std::map<int, std::string> response_messeges;

		//request stuff
		int* fd;
		int* method;
		std::string* requested_url;
		std::string* query_string;
		std::map <std::string, std::string>* params;

		int get_bodyfile_length();
		void execute_script();
		void execute_extention_script( const std::string& filename, char** cgi_envp );
		
		long find_cgi_file_length( std::ifstream& cgi_response_file, std::string& content_type );
		std::string get_extention()const;
		char** create_appended_envp();
		void init_new_envp( std::map<std::string, std::string>& );
		void start();
		void process();
		void write();
		void init_response_msgs();
		std::string generate_response_head( const int& code );
		// CGI_handler& operator=(const CGI_handler& a);
	};
}
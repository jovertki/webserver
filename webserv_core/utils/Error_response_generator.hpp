#pragma once
#include <map>
#include <string>

namespace ft {
	class Error_response_generator {
	public:
		Error_response_generator( const std::map<int, std::string>* response_msgs = NULL);
		~Error_response_generator();
		std::string generate_errorpage( const int& error_code, const std::string&, const bool& cease_connection = false )const;
	private:
		// Error_response_generator( const Error_response_generator& a );
		// Error_response_generator& operator=( const Error_response_generator& a );
		const std::map<int, std::string> *response_messeges;
	};
}
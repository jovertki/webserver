#include <string>
#include "../resources/defines.hpp"
#include <map>
#include <vector>
namespace ft {
	class Request {
	private:
		int method;
		std::string requested_url;
		std::string httpver;
		int header_length;
		std::vector<char> body;
		std::vector<char> args;
		std::map <std::string, std::string> params;
		std::string get_requested_filename() const;
	public:
		int get_method() const;
		std::string get_requested_url() const;
		std::string get_httpver() const;
		std::string get_requested_url_extention() const;
		std::string get_content_type() const;
		std::vector<char> get_body() const;
		std::vector<char> get_args() const;
		std::map <std::string, std::string>get_params()const;
		std::string get_param_value( const std::string& n );
		int get_header_length()const;
		
		void set_method( const int& );
		void set_requested_url(const std::string&);
		void set_httpver(const std::string&);
		void set_body( const std::vector<char>& );
		void set_body( const std::string& );
		void set_url_args( const std::string& );
		void set_body_args();
		void set_params( const std::map <std::string, std::string>& );
		void set_header_length( const int& );

		
		void insert_param( const std::pair<std::string, std::string>& );
		void print_params();
		int param_exists( const std::string& ) const;
	};
}
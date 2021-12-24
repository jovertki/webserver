#include <string>
#include "../resources/defines.hpp"
namespace ft {
	class Request {
	private:
		int method;
		std::string requested_url;
		std::string httpver;
		std::string body;
		std::string args;
	public:
		int get_method() const;
		std::string get_requested_url() const;
		std::string get_httpver() const;
		std::string get_requested_filename() const;
		std::string get_requested_url_extention() const;
		std::string get_content_type() const;
		std::string get_body() const;
		std::string get_args() const;

		void set_method( int );
		void set_requested_url(std::string);
		void set_httpver(std::string);
		void set_body( std::string );
		void set_url_args(std::string);
		void set_body_args();
	};
}
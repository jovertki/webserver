#include <string>
#include "../resources/defines.hpp"
#include <map>
#include <vector>
#include <fstream>
namespace ft {
	class Request {
	private:
		int method;
		std::string requested_url;
		std::string httpver;
		int header_length;
		std::string body_file_name;
		std::string query_string;
		std::map <std::string, std::string> params;


		long total_bytes_read;
		long full_request_length;
		std::string get_requested_filename() const;
	public:
		bool parsing_header;
		bool parsing_data_header;

		
		Request();
		int get_method() const;
		std::string get_requested_url() const;
		std::string get_httpver() const;
		std::string get_requested_url_extention() const;
		std::string get_content_type() const;
		// std::ofstream get_body_fd() const;
		std::string get_query_string() const;
		std::map <std::string, std::string>get_params()const;
		std::string get_param_value( const std::string& n );
		int get_header_length()const;
		std::map<std::string, std::string>::iterator get_params_begin();
		std::map<std::string, std::string>::iterator get_params_end();
		long get_total_bytes_read() const;
		long get_full_request_length() const;
		
		void set_method( const int& );
		void set_requested_url(const std::string&);
		void set_httpver(const std::string&);
		// void set_body( const std::ofstream& );
		void set_query_string( const std::string& );
		// void set_body_args();
		void set_params( const std::map <std::string, std::string>& );
		void set_param( const std::string& key, const std::string& value );
		void set_header_length( const int& );
		void set_total_bytes_read( const long& );
		void set_full_request_length( const long& );
		
		void insert_param( const std::pair<std::string, std::string>& );
		void print_params();
		int param_exists( const std::string& ) const;
		void clear();
	};
}
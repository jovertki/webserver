#include "utils.hpp"

namespace ft {
	bool is_directory( const std::string& path ) {
		struct stat s;
		if(stat( path.c_str(), &s ) == 0)
		{
			if(s.st_mode & S_IFDIR)
			{
				//it's a directory
				return 1;
			}
			else
			{
				//something else
				return 0;
			}
		}
		return 0;
	}
	long get_file_size( const std::string& filename ) {
		std::ifstream infile( filename );
		if(!infile.is_open()) {
			return -1;
		}
		infile.seekg( 0, std::ios::end );
		return infile.tellg();
	}
}



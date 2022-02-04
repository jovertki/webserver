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
}
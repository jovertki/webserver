#pragma once
#include <string>
#include <sys/stat.h>
#include <fstream>

namespace ft {
	bool is_directory( const std::string& path );
	long get_file_size( const std::string& filename );
}
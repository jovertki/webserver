#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <fstream>
#define FILENAME "/test_upload.txt"
#include <errno.h>
int main( int argc, char** argv, char** envp ) {
	std::string upload_path;
	for(int i = 0; envp[i] != NULL; i++) {
		if(strncmp( envp[i], "UPLOAD_PATH=", 12) == 0) {
			std::string crutch( envp[i] );
			upload_path = crutch.substr( 12 );
		}
	}
	std::ofstream outfile( upload_path + FILENAME );
	std::cout << strerror( errno ) << std::endl;
	std::cout << upload_path + FILENAME << std::endl;
	if(!outfile.is_open()) {
		std::cout << "error uploading" << std::endl;
	}
	std::string line;
	while(getline( std::cin, line ))
	{
		outfile << line << '\n';
	}
	outfile.close();
}
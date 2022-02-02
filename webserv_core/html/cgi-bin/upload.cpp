#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <errno.h>
int main( int argc, char** argv, char** envp ) {
	std::string upload_path;
	for(int i = 0; envp[i] != NULL; i++) {
		if(strncmp( envp[i], "UPLOAD_PATH=", 12) == 0) {
			std::string crutch( envp[i] );
			upload_path = crutch.substr( 12 );
		}
	}
	std::ofstream outfile( upload_path );
	if(!outfile.is_open()) {
		std::cout << upload_path << std::endl;
	}
	std::string line;
	while(getline( std::cin, line ))
	{
		outfile << line;
		if(!std::cin.eof())
			outfile << '\n';
	}
	outfile.close();

	std::cout << "Content-Type: text/html\r\n\r\n";
	std::cout << "<h1>File was successfully uploaded</h1>";
}
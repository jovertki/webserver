#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
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
		std::cout << "error uploading" << std::endl;
	}
	std::string line;
	while(getline( std::cin, line ))
	{
		outfile << line;
		if(!std::cin.eof())
			outfile << '\n';
	}
	outfile.close();

	int fd = open( upload_path.c_str(), O_RDONLY );
	write( 1, "Content-Type: image/vnd.microsoft.icon\r\n\r\n", strlen( "Content-Type: image/vnd.microsoft.icon\r\n\r\n" ) );

	
	char buffer[8001];
	bzero( buffer, 8000 );
	while(read(fd, buffer, 8000)){
		write( 1, buffer, 8000 );
		bzero( buffer, 8000 );
	}
	exit( 0 );
}
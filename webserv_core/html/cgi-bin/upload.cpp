#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <errno.h>
int main( int argc, char** argv, char** envp ) {
	std::string upload_path;
	for(int i = 0; envp[i] != NULL; i++) {
		if(strncmp( envp[i], "QUERY_STRING=", strlen( "QUERY_STRING=" ) ) == 0) {
			std::string crutch( envp[i] );
			upload_path = crutch.substr( strlen( "QUERY_STRING=" ) + strlen( "path=" ) );
			break;
		}
	}

	
	for(int i = 0; envp[i] != NULL; i++) {
		if(strncmp( envp[i], "UPLOAD_FILENAME=", strlen( "UPLOAD_FILENAME=" ) ) == 0) {
			std::string crutch( envp[i] );
			upload_path = upload_path + "/" + crutch.substr( strlen( "UPLOAD_FILENAME=" ) );
			break;
		}
	}

	std::cout << "Content-Type: text/html\r\n\r\n";
	
	std::ofstream outfile( upload_path );
	if(!outfile.is_open()) {
		std::cout << "<h1>Problem occured. Check folder existance</h1>" << std::endl;
		return 1;
	}
	std::string line;
	while(getline( std::cin, line ))
	{
		outfile << line;
		if(!std::cin.eof())
			outfile << '\n';
	}
	outfile.close();

	std::cout << "<h1>File was successfully uploaded</h1>";
}
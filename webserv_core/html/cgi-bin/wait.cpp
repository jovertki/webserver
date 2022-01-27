#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
int main( int argc, char** argv, char** envp ) {


	int i = 0;
	std::cout << "Content-Type: text/html\n\n";
	// std::cout << "<h1>wait is over</h1>" << std::endl;
	int seconds;
	std::string string;
	std::cin >> string;
	seconds = atoi( (string.substr( string.find( "=" ) + 1, string.size() - (string.find( "=" ) + 1) ).c_str()));
	
	sleep( seconds );
	std::cout << "<h1>Waited for " << seconds << " seconds" <<"</h1>" << std::endl;
}
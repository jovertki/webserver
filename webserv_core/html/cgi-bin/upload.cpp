#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>

int main( int argc, char** argv, char** envp ) {

	std::string text;
	std::cin >> text;
	std::cout << text << " SUCCESS" << std::endl;
	// char* buffer[30000] = { 0 };
	// while(read( socket, buffer, 30000 ) > 0) {
	// 	std::cout << buffer << std::endl;
	// }
}
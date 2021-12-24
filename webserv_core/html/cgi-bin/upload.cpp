#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>

int main( int argc, char** argv, char** envp ) {
	int socket = 0;
	for(int i = 0; envp[i] != NULL; i++) {
		std::cout << static_cast<std::string>(envp[i]) << std::endl;
		std::string s;
		if((s = static_cast<std::string>(envp[i])).find( "SOCKET=" ) == 0) {
			std::cout << "!!!!!!!here is cgi output:" << s << std::endl;
			socket = atoi( (static_cast<std::string>(envp[i]).substr( 7 )).c_str() );
			break;
		}
	}
	std::cout << "YEUP" << std::endl;
	char* buffer[30000] = { 0 };
	while(read( socket, buffer, 30000 ) > 0) {
		std::cout << buffer << std::endl;
	}
}
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>

int main( int argc, char** argv, char** envp ) {	
	for(int i = 0; envp[i] != NULL; i++) {
		if(strncmp( envp[i], "UPLOAD_PATH=", 12) == 0) {
			std::string upload_path( envp[i] );
			upload_path = upload_path.substr( 12 );
		}
	}

	
}
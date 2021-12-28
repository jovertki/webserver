// // Server side C program to demonstrate Socket programming
// #include <stdio.h>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <netinet/in.h>
// #include <string.h>
// #include <string>
// #include <fcntl.h>
// #include <iostream>
// #include <fstream>

// #define PORT 8080

// void sigint_handle( int server_fd ) {
// 	close( server_fd );
// 	exit( 1 );
// }

// int main( int argc, char const* argv[] )
// {
// 	int server_fd, new_socket; long valread;
// 	struct sockaddr_in address;
// 	int addrlen = sizeof( address );
// 	int file_fd = open( "todolist.txt", O_RDONLY );

// 	std::ifstream infile;
// 	infile.open( "todolist.txt" );
// 	std::string buff = "<b>Pick your favorite color</b><br>\n<form method = \"POST\" action = \"http://www.whizkidtech.redprince.net/cgi-bin/c\">\n<input type = \"RADIO\" name = \"color\" value = \"red\"> Red<br>\n<input type = \"RADIO\" name = \"color\" value = \"green\"> Green<br>\n<input type = \"RADIO\" checked name = \"color\" value = \"blue\"> Blue<br>\n<input type = \"RADIO\" name = \"color\" value = \"cyan\"> Cyan<br>\n<input type = \"RADIO\" name = \"color\" value = \"magenta\"> Magenta<br>\n<input type = \"RADIO\" name = \"color\" value = \"yellow\"> Yellow<br>\n<br><b>On the scale 1 - 3, how favorite is it ? </b><br><br>\n<select name = \"scale\" size = 1>\n< option>1\n< option selected > 2\n< option > 3\n</select >\n<br>\n<input type = \"HIDDEN\" name = \"favorite color\" size = \"32\">\n<input type = \"Submit\" value = \"I'm learning\" name = \"Attentive student\">\n<input type = \"Submit\" value = \"Give me a break!\" name = \"Overachiever\">\n<input type = \"Reset\" name = \"Reset\">\n</form>";

// 	std::cout << buff << std::endl;
// 	std::cout << "here" << std::endl;
// 	std::string hello1 = "HTTP/1.1 200 OK\nContent-Type: text/html\n";
// 	hello1 += "Content-Length: " + std::to_string(buff.size()) + "\n\n";
// 		std::string hello = hello1 + buff;
// 	std::cout << hello << std::endl;

// 	// Creating socket file descriptor
// 	if((server_fd = socket( AF_INET, SOCK_STREAM, 0 )) == 0)
// 	{
// 		perror( "In socket" );
// 		exit( EXIT_FAILURE );
// 	}


// 	address.sin_family = AF_INET;
// 	address.sin_addr.s_addr = INADDR_ANY;
// 	address.sin_port = htons( PORT );

// 	memset( address.sin_zero, '\0', sizeof address.sin_zero );


// 	if(bind( server_fd, (struct sockaddr*)&address, sizeof( address ) ) < 0)
// 	{
// 		perror( "In bind" );
// 		exit( EXIT_FAILURE );
// 	}
// 	if(listen( server_fd, 10 ) < 0)
// 	{
// 		perror( "In listen" );
// 		exit( EXIT_FAILURE );
// 	}
// 	signal( SIGINT, sigint_handle );
// 	while(1)
// 	{
// 		printf( "\n+++++++ Waiting for new connection ++++++++\n\n" );
// 		if((new_socket = accept( server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen )) < 0)
// 		{
// 			perror( "In accept" );
// 			exit( EXIT_FAILURE );
// 		}

// 		char buffer[30000] = { 0 };
// 		valread = read( new_socket, buffer, 30000 );
// 		printf( "%s\n", buffer );
// 		write( new_socket, hello.c_str(), hello.size() );
// 		printf( "------------------Hello message sent-------------------\n" );
// 		close( new_socket );
// 	}
// 	return 0;
// }
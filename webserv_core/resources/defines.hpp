#pragma once
#define SERVER_DIR "./html"
#define ERROR404FILE "./html/error404.html"
#define MIME_FILE "./resources/mime.types"
#define BUFFER_FILE "./runtime_files/bufferfile"
#define BUFFER_FILE_OUT "./runtime_files/bufferfileout"
#define BUFFER_FILE_CGIOUT "./runtime_files/bufferfilecgiout"



enum method {
	EMPTY,
	GET,
	POST,
	DELETE,
	PUT
};

//request stages
#define REQUEST_PENDING 0
#define REQUEST_READ 1
#define REQUEST_GENERATED 2


//cgi stages
#define CGI_NOT_STARTED 0
#define CGI_PROCESSING 1
#define CGI_FINISHED 2
#define CGI_BUFFER_SIZE 8000
//colored output
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
#include <string>
#include <sstream>
#include <iostream>
int main( int argc, char** argv, char** envp ) {


	int i = 0;
	std::cout << "Content-Type: text/html\r\n\r\n";
	std::string s;
	std::cin >> s;
	std::cout << "<h1>This was calculated with cpp binary and POST request</h1>" << std::endl;
	std::cout << "<h1>Addition Results</h1>" << std::endl;
	// std::cout << "<output>" << instr << "</output>\n<br>" << std::endl;
	for(int i = 0; envp[i] != NULL; i++) {
		// if((s = static_cast<std::string>(envp[i])).find( "ARGS=" ) == 0) {
			std::size_t pos = s.find( "num1=" );
			if(pos == std::string::npos) {
				std::cout << "<output>Sorry, the script cannot turn your inputs into numbers (integers).</output>" << std::endl;
				exit( 1 );
			}
			std::string substr = s.substr( pos + 5 );
			std::istringstream iss( substr );
			int num1;
			iss >> num1;
			pos = s.find( "num2=" );
			if(pos == std::string::npos) {
				std::cout << "<output>Sorry, the script cannot turn your inputs into numbers (integers).</output>" << std::endl;
				exit( 1 );
			}
			substr = s.substr( pos + 5 );
			std::istringstream iss2( substr );
			int num2;
			iss2 >> num2;

			int out = num1 + num2;
			std::cout << "<output>" << num1 << " + " << num2 << " = " << num1 + num2 << "</output>";
			return 0;
		// }
	}
	std::cout << "<output>Sorry, the script cannot turn your inputs into numbers (integers).</output>" ;
	exit( 0);

	
}
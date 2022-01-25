#include "Utils_parse_config.hpp"

namespace utils {

    parseExeption::parseExeption(const std::string &msg) : m_msg(msg) {}

    parseExeption::~parseExeption() throw() {}

    const char *parseExeption::parseExeption::what() const throw() {
//        std::cerr << "ServerParse: " << m_msg << std::endl;
        return (m_msg.c_str());
    }

    int str_to_num(const std::string& to_convert) {
        long int res;
        char *end_c;

        res = strtol(to_convert.c_str(), &end_c, 10);
        if ((errno == ERANGE && res == LONG_MAX) || res > INT_MAX ||
            (errno == ERANGE && res == LONG_MIN) || res < 0
            || to_convert.empty() || *end_c != '\0')
            throw utils::parseExeption("Error str_to_num!");
        return (int)res;
    }
    std::vector<std::string> make_tokens(std::ifstream& input) {
        std::string oneToken;
        std::vector<std::string> tokens;

        char c;
        if (!input.is_open())
            throw utils::parseExeption("Error config: can't open file ");
        while (input.good())
        {
            c = input.get();
            if (std::isspace(c) || c == ';' || c == '\n' || c == EOF) {
                if (!oneToken.empty()) {
                    tokens.push_back(std::string(oneToken));
//                    std::cout << oneToken << std::endl; // delete
                    oneToken.clear();
                }
                if (c == ';')
                    tokens.push_back(std::string(&c, 1));
            }
            else if (c == '#'){
                input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (input.eof())
                    c = EOF;
            }
            else
                oneToken.push_back(c);
        }
        if (c != EOF || tokens.empty() || tokens.size() < 3)
            throw utils::parseExeption("Error make_tokens!");
        else
            input.close();
        return tokens;
    }

}
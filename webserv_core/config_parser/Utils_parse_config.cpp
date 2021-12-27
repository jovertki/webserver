#include "Utils_parse_config.hpp"

namespace utils {

    MyException::MyException(const std::string &msg) : m_msg(msg) {}

    MyException::~MyException() throw() {}

    const char *MyException::MyException::what() const throw() {
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
            throw utils::MyException("Error str_to_num!");
        return (int)res;
    }
    std::vector<std::string> make_tokens(std::ifstream& input) {
        std::string oneToken;
        std::vector<std::string> tokens;
        char c;
        while (input.good())
        {
            c = input.get();
            if (std::isspace(c) || c == ';' || c == '\n' || c == EOF) {
                if (!oneToken.empty()) {
                    tokens.push_back(std::string(oneToken));
                    oneToken.clear();
                }
                if (c == ';')
                    tokens.push_back(std::string(&c, 1));
            }
            else if (c == '#')
                input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            else
                oneToken.push_back(c);
        }
        if (c != EOF || tokens.empty() || tokens.size() < 3)
            throw utils::MyException("Error make_tokens!");
        else
            input.close();
        return tokens;
    }

}
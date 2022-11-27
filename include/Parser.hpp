#ifndef BIS_PARSER_HPP
#define BIS_PARSER_HPP

#include "Token.hpp"
#include <vector>

class Parser
{
public:
    Parser(std::vector<Token>& tokens);

private:
    unsigned int current;
    std::vector<Token> tokens;
};

#endif // BIS_PARSER_HPP
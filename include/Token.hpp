#ifndef BIS_TOKEN_HPP
#define BIS_TOKEN_HPP
#include "TokenType.hpp"
#include <string>

struct Token
{
    Token(const TokenType type, const std::string lexeme, const unsigned int line);
    const TokenType type;
    const std::string lexeme;
    const unsigned int line;
};

std::ostream& operator<<(std::ostream& os, const Token& token);
std::ostream& operator<<(std::ostream& os, const TokenType type);

#endif // BIS_TYPES_TOKEN_HPP

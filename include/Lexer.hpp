#ifndef BIS_LEXER_HPP
#define BIS_LEXER_HPP

#include "../include/Token.hpp"
#include <unordered_map>
#include <vector>
using TokenType = Token::TokenType;
class Lexer
{
public:
    Lexer(std::string_view source);
    std::vector<Token>& scanTokens();

private:
    const std::string source;
    std::vector<Token> tokens;
    unsigned int start, current, line;
    static const std::unordered_map<std::string, TokenType> keywords;

    bool isEOF() const;
    bool isDigit(const char c) const;
    bool isAlpha(const char c) const;
    bool isAlphaNumeric(const char c) const;
    bool match(const char expected);
    std::string getLexeme(TokenType type) const;

    void advance();
    char peek() const;
    char peekNext() const;

    void addToken(TokenType type);
    void scanToken();
    void string();
    void number();
    void identifier();
};

#endif // BIS_LEXER__HPP
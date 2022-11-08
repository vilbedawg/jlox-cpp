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
    std::string m_source;
    size_t start, current, line;
    std::vector<Token> m_tokens;
    const std::unordered_map<std::string, TokenType> keywords;

    bool isEOF() const;
    bool isDigit(const char c) const;
    bool isAlpha(const char c) const;
    bool isAlphaNumeric(const char c) const;
    bool match(const char expected);

    char advance();
    char peek() const;
    char peekNext() const;

    void addToken(TokenType type);
    void addToken(TokenType type, literalType literal);

    void scanToken();
    void string();
    void number();
    void identifier();
};

#endif
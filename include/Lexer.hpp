#ifndef BIS_LEXER_HPP
#define BIS_LEXER_HPP

#include "../include/Token.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>
class Lexer
{
public:
    Lexer(std::string_view source);
    std::vector<Token>& scanTokens();

private:
    std::string m_source;
    size_t start, current, line;
    std::vector<Token> m_tokens;
    const std::unordered_map<std::string, Token::TokenType> keywords;

    bool isEOF() const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool match(char expected);

    char advance();
    char peek() const;
    char peekNext() const;

    void addToken(Token::TokenType type);
    void addToken(Token::TokenType type, literalType literal);

    void scanToken();
    void string();
    void number();
    void identifier();
};

#endif
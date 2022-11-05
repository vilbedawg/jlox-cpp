#ifndef BIS_LEXER_HPP
#define BIS_LEXER_HPP

#include <vector>
#include <map>
#include <any>
#include <string>
#include <iostream>
#include "../include/Token.hpp"
class Lexer
{
public:
    Lexer(std::string_view source) : m_source{source}
    {
    }
    std::vector<Token> scanTokens();
    bool isEOF();
    void printToken(Token token); 

private:
    const std::string m_source;
    size_t start{0}, current{0}, line{1};
    std::vector<Token> m_tokens;
    const std::map<std::string, Token::TokenType> keywords{
        {"and",     Token::TokenType::AND},
        {"or",      Token::TokenType::OR},
        {"class",   Token::TokenType::CLASS},
        {"if",      Token::TokenType::IF},
        {"else",    Token::TokenType::ELSE},
        {"false",   Token::TokenType::BIS_FALSE},
        {"true",    Token::TokenType::BIS_TRUE},
        {"fn",      Token::TokenType::FN},
        {"for",     Token::TokenType::FOR},
        {"while",   Token::TokenType::WHILE},
        {"nil",     Token::TokenType::NIL},
        {"print",   Token::TokenType::PRINT},
        {"return",  Token::TokenType::RETURN},
        {"super",   Token::TokenType::SUPER},
        {"this",    Token::TokenType::THIS},
        {"token",   Token::TokenType::VAR},
        {"lambda",  Token::TokenType::LAMBDA}
    };

    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    bool isDigit(char c);
    bool match(char excepted);
    
    char advance();
    char peek(); 
    char peekNext();

    void scanToken();
    void addToken(Token::TokenType type);
    void addToken(Token::TokenType type, std::any literal);
    void makeString();
    void makeNumber();
    void makeIdentifier();
};

#endif
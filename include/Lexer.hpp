#ifndef LEXER_HPP
#define LEXER_HPP

#include "Token.hpp"
#include <unordered_map>
#include <vector>

class Lexer
{
public:
    explicit Lexer(std::string source);

    std::vector<Token> scanTokens();

    static const std::unordered_map<std::string, TokenType> keywords;

private:
    const std::string source;
    std::vector<Token> tokens;
    unsigned int start = 0;
    unsigned int current = 0;
    unsigned int line = 1;

    bool isEOF() const;

    bool isDigit(char c) const;

    bool isAlpha(char c) const;

    bool isAlphaNumeric(char c) const;

    bool match(char expected);

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

#endif // LEXER_HPP
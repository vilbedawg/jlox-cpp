#ifndef BIS_TYPES_TOKEN_H
#define BIS_TYPES_TOKEN_H
#include <map>
#include <string>
#include <variant>

using literalType = std::variant<std::string, double>;
struct Token
{
    enum class TokenType
    {
        // Single-character tokens
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RUGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SLASH,
        SEMICOLON,
        STAR,
        LEFT_BRACET,

        // One or two character tokens
        RIGHT_BRACKET,
        EXCLAMATION,
        EXCLAMATION_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        MINUS_MINUS,
        PLUS_PLUS,

        // Literal
        IDENTIFIER,
        STRING,
        NUMBER,

        // Keyword
        AND,
        OR,
        CLASS,
        IF,
        ELSE,
        BIS_FALSE,
        BIS_TRUE,
        FN,
        FOR,
        WHILE,
        NIL,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        VAR,
        LAMBDA,

        _EOF
    };

    Token(const TokenType type, const std::string lexeme, literalType literal, const size_t line);
    std::string toString() const;
    literalType m_literal;
    const TokenType m_type;
    const std::string m_lexeme;
    const size_t m_line;
};

#endif
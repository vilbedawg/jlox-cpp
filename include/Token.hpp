#ifndef BIS_TYPES_TOKEN_H
#define BIS_TYPES_TOKEN_H
#include <string>

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

    Token(TokenType type, size_t line) noexcept : m_type{type}, m_line{line} {}

    Token(TokenType type, std::string_view lexeme, size_t line) noexcept
        : m_type{type}, m_lexeme{lexeme}, m_line{line} {}

    const TokenType m_type;
    const std::string m_lexeme;
    size_t m_line;
};

#endif
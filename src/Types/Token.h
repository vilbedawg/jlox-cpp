#ifndef BIS_TYPES_TOKEN_H
#define BIS_TYPES_TOKEN_H

#include <string>
#include <iostream>
namespace bis
{
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
            // One or two character tokens
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
            // Literals
            IDENTIFIER,
            STRING,
            NUMBER,
            // Keywords
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

            _EOF
        };

        Token(TokenType type, int line) noexcept : m_type{type}, m_line{line} {}

        Token(TokenType type, std::string_view lexeme, int line) noexcept
            : m_type{type}, m_lexeme{lexeme}, m_line{line} {}

        const TokenType m_type;
        const std::string m_lexeme;
        const int m_line = -1;
    };
}
#endif
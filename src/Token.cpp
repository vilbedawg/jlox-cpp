#include "../include/Token.hpp"
#include <algorithm>
#include <iostream>
#include <map>

using TokenType = Token::TokenType;

Token::Token(const TokenType type, const std::string lexeme, const unsigned int line)
    : type{type}, lexeme{std::move(lexeme)}, line{line}
{
}

std::ostream& operator<<(std::ostream& os, const Token::TokenType type)
{
    static const std::map<TokenType, std::string> type_map{
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::STAR, "STAR"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::EXCLAMATION, "EXCLAMATION"},
        {TokenType::EXCLAMATION_EQUAL, "EXCLAMATION_EQUAL"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::MINUS_MINUS, "MINUS_MINUS"},
        {TokenType::PLUS_PLUS, "PLUS_PLUS"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING, "STRING"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::AND, "AND"},
        {TokenType::OR, "OR"},
        {TokenType::CLASS, "CLASS"},
        {TokenType::IF, "IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::_FALSE, "_FALSE"},
        {TokenType::_TRUE, "_TRUE"},
        {TokenType::FN, "FN"},
        {TokenType::FOR, "FOR"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::NIL, "NIL"},
        {TokenType::PRINT, "PRINT"},
        {TokenType::RETURN, "RETURN"},
        {TokenType::BREAK, "BREAK"},
        {TokenType::SUPER, "SUPER"},
        {TokenType::THIS, "THIS"},
        {TokenType::VAR, "VAR"},
        {TokenType::LAMBDA, "LAMBDA"},
        {TokenType::_EOF, "EOF"},
    };

    os << type_map.at(type);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << token.type << ' ' << token.lexeme;
    return os;
}
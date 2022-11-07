#include "../include/Token.hpp"

Token::Token(const TokenType type, const std::string lexeme, literalType literal, const size_t line)
    : m_type{type}, m_lexeme{lexeme}, m_literal{literal}, m_line{line}
{
}

// std::string Token::toString() const
//{
// static const std::map<TokenType, std::string> tokens{
//{TokenType::LEFT_PAREN, "LEFT_PAREN"},
//{TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
//{TokenType::LEFT_BRACE, "LEFT_BRACE"},
//{TokenType::RUGHT_BRACE, "RUGHT_BRACE"},
//{TokenType::COMMA, "COMMA"},
//{TokenType::DOT, "DOT"},
//{TokenType::MINUS, "MINUS"},
//{TokenType::PLUS, "PLUS"},
//{TokenType::SLASH, "SLASH"},
//{TokenType::SEMICOLON, "SEMICOLON"},
//{TokenType::STAR, "STAR"},
//{TokenType::LEFT_BRACET, "LEFT_BRACET"},
//{TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
//{TokenType::EXCLAMATION, "EXCLAMATION"},
//{TokenType::EXCLAMATION_EQUAL, "EXCLAMATION_EQUAL"},
//{TokenType::EQUAL, "EQUAL"},
//{TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
//{TokenType::GREATER, "GREATER"},
//{TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
//{TokenType::LESS, "LESS"},
//{TokenType::LESS_EQUAL, "LESS_EQUAL"},
//{TokenType::MINUS_MINUS, "MINUS_MINUS"},
//{TokenType::PLUS_PLUS, "PLUS_PLUS"},
//{TokenType::IDENTIFIER, "IDENTIFIER"},
//{TokenType::STRING, "STRING"},
//{TokenType::NUMBER, "NUMBER"},
//{TokenType::AND, "AND"},
//{TokenType::OR, "OR"},
//{TokenType::CLASS, "CLASS"},
//{TokenType::IF, "IF"},
//{TokenType::ELSE, "ELSE"},
//{TokenType::BIS_FALSE, "BIS_FALSE"},
//{TokenType::BIS_TRUE, "BIS_TRUE"},
//{TokenType::FN, "FN"},
//{TokenType::FOR, "FOR"},
//{TokenType::WHILE, "WHILE"},
//{TokenType::NIL, "NIL"},
//{TokenType::PRINT, "PRINT"},
//{TokenType::RETURN, "RETURN"},
//{TokenType::SUPER, "SUPER"},
//{TokenType::THIS, "THIS"},
//{TokenType::VAR, "VAR"},
//{TokenType::LAMBDA, "LAMBDA"},
//{TokenType::_EOF, "EOF"},
//};

// return std::to_string(tokens.at(m_type) + ", lexeme: '" + m_lexeme + "' , literal: '" +
// m_literal + "'");
//}

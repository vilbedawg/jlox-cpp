#ifndef PARSER_HPP
#define PARSER_HPP

#include "../include/Logger.hpp"
#include "ExprNode.hpp"
#include "StmtNode.hpp"
#include "Token.hpp"
#include "Typedef.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

class Parser
{
public:
    explicit Parser(std::vector<Token> tokens);

    std::vector<unique_stmt_ptr> parse();

private:
    std::vector<Token> tokens;
    unsigned int current = 0;

    unique_stmt_ptr declaration();

    unique_stmt_ptr classDecl();

    unique_stmt_ptr varDeclaration();

    unique_stmt_ptr statement();

    unique_stmt_ptr function(const std::string& kind);

    unique_stmt_ptr ifStatement();

    unique_stmt_ptr forStatement();

    unique_stmt_ptr whileStatement();

    std::vector<unique_stmt_ptr> block();

    unique_stmt_ptr expressionStatement();

    unique_expr_ptr expression();

    unique_expr_ptr assignment();

    unique_expr_ptr lambda();

    std::vector<unique_expr_ptr> list();

    unique_expr_ptr subscript();

    unique_expr_ptr finishSubscript(unique_expr_ptr identifier);

    unique_expr_ptr orExpression();

    unique_expr_ptr andExpression();

    unique_expr_ptr equality();

    unique_expr_ptr comparison();

    unique_expr_ptr term();

    unique_expr_ptr factor();

    unique_expr_ptr unary();

    unique_expr_ptr prefix();

    unique_expr_ptr postfix();

    unique_expr_ptr call();

    unique_expr_ptr finishCall(unique_expr_ptr callee);

    unique_expr_ptr primary();

    unique_stmt_ptr printStatement();

    unique_stmt_ptr returnStatement();

    unique_stmt_ptr forInitializer();

    unique_expr_ptr forExpression(TokenType type, std::string msg);

    unique_stmt_ptr controlStatement();

    template <typename Fn>
    unique_expr_ptr binary(Fn func, const std::initializer_list<TokenType>& token_args);

    bool isAtEnd() const;

    bool match(const std::initializer_list<TokenType> args);

    bool check(TokenType type) const;

    void consume(TokenType type, std::string msg);

    void synchronize();

    void advance();

    const Token& peek() const;

    const Token& previous() const;

    class ParseError : public std::runtime_error
    {
    public:
        ParseError() : std::runtime_error("") {}
    };

    ParseError error(const Token& token, std::string message) const;
};

#endif // PARSER_HPP
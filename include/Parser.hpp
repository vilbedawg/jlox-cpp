#ifndef BIS_PARSER_HPP
#define BIS_PARSER_HPP

#include "../include/Logger.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
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

    unique_expr_ptr expression();
    unique_expr_ptr equality();
    unique_expr_ptr assignment();
    unique_expr_ptr orExpression();
    unique_expr_ptr andExpression();

    unique_expr_ptr comparison();
    unique_expr_ptr term();
    unique_expr_ptr factor();
    unique_expr_ptr unary();

    unique_expr_ptr call();
    unique_expr_ptr finishCall(unique_expr_ptr callee);
    unique_expr_ptr primary();
    unique_expr_ptr prefix();
    unique_expr_ptr postfix();
    unique_expr_ptr list();

    unique_stmt_ptr statement();
    unique_stmt_ptr declaration();
    unique_stmt_ptr varDeclaration();
    unique_stmt_ptr printStatement();
    unique_stmt_ptr expressionStatement();
    std::vector<unique_stmt_ptr> block();
    unique_stmt_ptr ifStatement();
    unique_stmt_ptr whileStatement();
    unique_stmt_ptr forStatement();
    unique_stmt_ptr forInitializer();
    unique_stmt_ptr controlStatement();
    unique_expr_ptr forExpression(TokenType type, std::string msg);

    unique_stmt_ptr function(const std::string& kind);

    template <typename Fn>
    unique_expr_ptr binary(Fn func, const std::initializer_list<TokenType>& token_args);

    bool match(const std::initializer_list<TokenType> args);
    bool check(TokenType type) const;
    bool isAtEnd() const;
    void synchronize();
    void advance();
    void expect(TokenType type, std::string msg);

    const Token& peek() const;
    const Token& previous() const;

    class ParseError : public std::runtime_error
    {
    public:
        ParseError() : std::runtime_error("") {}
    };

    ParseError error(const Token& token, std::string message) const;
};

#endif // BIS_PARSER_HPP
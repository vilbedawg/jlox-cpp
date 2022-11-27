#include "../include/Parser.hpp"

/**
 *  Each productions body roughly translates to the following
 *
 *  Grammar notation -> Code representation
 *  ----------------------------------------
 *  Terminal            Code to match and consume a token
 *  Nonterminal         Call to that rule's function
 *  |                   if or switch statement
 *  * or +              while or for loop
 *  ?                   if statement
 */

Parser::Parser(const std::vector<Token>& tokens) : tokens{tokens}
{
}

std::vector<unique_expr_ptr> Parser::parse()
{
    // Return a vector of expressions for now.
    // TODO: Implement statements.
    std::vector<unique_expr_ptr> expressions;
    try
    {
        while (!isAtEnd())
        {
            expressions.emplace_back(expression());
        }
        return expressions;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

/**
 * Binary operators
 */
unique_expr_ptr Parser::expression()
{
    // expression -> equality ;
    return equality();
}

unique_expr_ptr Parser::equality()
{
    // equality -> comparison ( ( "!=" | "==" ) comparison )* ;
    auto expr = comparison();
    while (match(TokenType::EXCLAMATION_EQUAL, TokenType::EQUAL_EQUAL))
    {
        auto _operator = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::comparison()
{
    // comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
    auto expr = term();

    while (
        match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL))
    {
        auto _operator = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::term()
{
    // term -> factor ( (  "-" | "+" ) factor )* ;
    auto expr = factor();

    while (match(TokenType::MINUS, TokenType::PLUS))
    {
        auto _operator = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::factor()
{
    // factor → unary ( ( "/" | "*" ) unary )* ;
    auto expr = unary();

    while (match(TokenType::SLASH, TokenType::STAR))
    {
        auto _operator = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), _operator, std::move(right));
    }

    return expr;
}

/**
 * Unary operators
 */

unique_expr_ptr Parser::unary()
{
    // unary -> ( "!" | "-" ) unary | prefix ;
    if (match(TokenType::EXCLAMATION, TokenType::MINUS))
    {
        auto _operator = previous();
        auto right = unary();
        return std::make_unique<UnaryExpr>(_operator, std::move(right));
    }

    return prefix();
}

unique_expr_ptr Parser::prefix()
{
    // prefix -> ( "++" | "--" ) prefix | lvalue | postfix;
    if (match(TokenType::PLUS_PLUS, TokenType::MINUS_MINUS))
    {
        auto _operator = previous();
        auto right = prefix();
        if (dynamic_cast<IncrementExpr*>(right.get()) || dynamic_cast<DecrementExpr*>(right.get()))
        {
            throw error(peek(), "Operators '++' and '--' cannot be concatenated.");
        }

        auto lvalue = dynamic_cast<VarExpr*>(right.get());

        if (lvalue)
        {
            std::unique_ptr<VarExpr> identifier{static_cast<VarExpr*>(right.release())};

            if (_operator.type == TokenType::PLUS_PLUS)
            {
                return std::make_unique<IncrementExpr>(std::move(identifier),
                                                       IncrementExpr::Type::PREFIX);
            }
            else
            {
                return std::make_unique<DecrementExpr>(std::move(identifier),
                                                       DecrementExpr::Type::PREFIX);
            }
        }

        throw error(_operator, "Operators '++' and '--' must be applied to and lvalue operand.");
    }

    return postfix();
}

unique_expr_ptr Parser::postfix()
{
    // postfix -> call ( ( "++" | "--" ) lvalue );
    auto expr = call();
    if (match(TokenType::PLUS_PLUS, TokenType::MINUS_MINUS))
    {
        Token _operator = previous();
        VarExpr* lvalue = dynamic_cast<VarExpr*>(expr.get());
        if (lvalue)
        {
            std::unique_ptr<VarExpr> identifier{static_cast<VarExpr*>(expr.release())};
            if (_operator.type == TokenType::PLUS_PLUS)
            {
                expr = std::make_unique<IncrementExpr>(std::move(identifier),
                                                       IncrementExpr::Type::POSTFIX);
            }
            else
            {
                expr = std::make_unique<DecrementExpr>(std::move(identifier),
                                                       DecrementExpr::Type::POSTFIX);
            }
        }

        throw error(_operator, "Operators '++' and '--' must be applied to and lvalue operand.");
    }

    if (match(TokenType::PLUS_PLUS, TokenType::MINUS_MINUS))
    {
        throw error(peek(), "Operators '++' and '--' cannot be concatenated.");
    }

    return expr;
}

unique_expr_ptr Parser::call()
{
    // call -> primary ( "(" arguments? ")" )* ;
    auto expr = primary();

    while (true)
    {
        if (match(TokenType::LEFT_PAREN))
        {
            expr = finishCall(std::move(expr));
        }
        else
        {
            break;
        }
    }

    return expr;
}

unique_expr_ptr Parser::finishCall(unique_expr_ptr callee)
{
}

// unique_expr_ptr Parser::list()
//{
//     return nullptr;
// }

unique_expr_ptr Parser::primary()
{
    /**
     * primary  -> LIST | NUMBER | STRING | "true" | "false" | "nil"
     *             IDENTIFIER |  "(" expression ")" ;
     */
    // if (match(TokenType::LEFT_BRACKET))
    // {
    //     return list();
    // }
    if (match(TokenType::NUMBER))
    {
        return std::make_unique<LiteralExpr>(std::strtod(previous().lexeme.c_str(), nullptr));
    }

    if (match(TokenType::STRING))
    {
        return std::make_unique<LiteralExpr>(previous().lexeme);
    }

    if (match(TokenType::_TRUE))
    {
        return std::make_unique<LiteralExpr>(true);
    }

    if (match(TokenType::_FALSE))
    {
        return std::make_unique<LiteralExpr>(false);
    }

    if (match(TokenType::NIL))
    {
        return std::make_unique<LiteralExpr>(std::any{});
    }

    if (match(TokenType::IDENTIFIER))
    {
        return std::make_unique<VarExpr>(previous());
    }

    if (match(TokenType::LEFT_PAREN))
    {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}

template <typename... Args>
bool Parser::match(Args... args)
{
    std::initializer_list<bool> results{check(args)...};
    for (auto is_match : results)
    {
        if (is_match)
        {
            static_cast<void>(advance());
            return true;
        }
    }

    return false;
}

Token Parser::consume(TokenType type, std::string msg)
{
    if (check(type))
    {
        return advance();
    }

    throw error(peek(), msg);
}

bool Parser::check(TokenType type) const
{
    if (isAtEnd())
    {
        return false;
    }

    return peek().type == type;
}

Token Parser::advance()
{
    if (!isAtEnd())
    {
        current++;
    }

    return previous();
}

bool Parser::isAtEnd() const
{
    return peek().type == TokenType::_EOF;
}

Token Parser::peek() const
{
    return tokens[current];
}

Token Parser::previous() const
{
    return tokens[current - 1];
}

Parser::ParseError Parser::error(const Token& token, std::string msg) const
{
    Error::add(token, std::move(msg));
    return ParseError();
}

void Parser::synchronize()
{
    static_cast<void>(advance());

    while (!isAtEnd())
    {
        if (previous().type == TokenType::SEMICOLON)
            return;

        switch (peek().type)
        {
        case TokenType::CLASS:
        case TokenType::FN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
        case TokenType::BREAK: return;
        default: break;
        }

        static_cast<void>(advance());
    }
}
#include "../include/Parser.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens{std::move(tokens)}
{
}

std::vector<unique_stmt_ptr> Parser::parse()
{
    std::vector<unique_stmt_ptr> statements;
    while (!isAtEnd())
    {
        statements.emplace_back(declaration());
    }

    return statements;
}

unique_stmt_ptr Parser::statement()
{
    if (match({TokenType::FOR}))
        return forStatement();
    if (match({TokenType::IF}))
        return ifStatement();
    if (match({TokenType::PRINT}))
        return printStatement();
    if (match({TokenType::WHILE}))
        return whileStatement();
    if (match({TokenType::LEFT_BRACE}))
        return std::make_unique<BlockStmt>(block());
    if (match({TokenType::BREAK, TokenType::CONTINUE}))
        return controlStatement();

    return expressionStatement();
}

unique_stmt_ptr Parser::controlStatement()
{
    auto token = previous();
    unique_stmt_ptr stmt;
    if (token.type == TokenType::BREAK)
    {
        consume(TokenType::SEMICOLON, "Expect ';' after break.");
        stmt = std::make_unique<BreakStmt>(std::move(token));
    }

    else if (token.type == TokenType::CONTINUE)
    {
        consume(TokenType::SEMICOLON, "Expect ';' after continue.");
        stmt = std::make_unique<ContinueStmt>(std::move(token));
    }

    return stmt;
}

unique_stmt_ptr Parser::forInitializer()
{
    if (match({TokenType::SEMICOLON}))
    {
        return nullptr;
    }
    else if (match({TokenType::VAR}))
    {
        return varDeclaration();
    }
    else
    {
        return expressionStatement();
    }
}

unique_expr_ptr Parser::forExpression(TokenType type, std::string msg)
{
    unique_expr_ptr expr;
    if (!check(type))
    {
        expr = expression();
    }

    consume(type, std::move(msg));

    return expr;
}

unique_stmt_ptr Parser::forStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
    auto initializer = forInitializer();
    auto condition = forExpression(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    auto increment = forExpression(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
    auto body = statement();

    return std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                     std::move(increment), std::move(body));
}

unique_stmt_ptr Parser::ifStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto if_condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after 'if condition.");
    auto then_statement = statement();

    IfBranch main_branch{std::move(if_condition), std::move(then_statement)};
    std::vector<IfBranch> elif_branches;

    while (match({TokenType::ELIF}))
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'elif'.");
        auto elif_condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after 'elif'.");
        auto elif_statement = statement();
        elif_branches.emplace_back(std::move(elif_condition), std::move(elif_statement));
    }

    unique_stmt_ptr else_branch;
    if (match({TokenType::ELSE}))
    {
        else_branch = statement();
    }

    return std::make_unique<IfStmt>(std::move(main_branch), std::move(elif_branches),
                                    std::move(else_branch));
}

unique_stmt_ptr Parser::declaration()
{
    try
    {
        if (match({TokenType::VAR}))
            return varDeclaration();
        if (match({TokenType::FN}))
            return function("function");

        return statement();
    }
    catch (ParseError&)
    {
        synchronize();
        return nullptr;
    }
}

unique_stmt_ptr Parser::printStatement()
{
    auto value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_unique<PrintStmt>(std::move(value));
}

unique_stmt_ptr Parser::varDeclaration()
{
    consume(TokenType::IDENTIFIER, "Expect variable name.");
    auto identifier = previous();
    auto initializer = match({TokenType::EQUAL}) ? expression() : nullptr;
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");

    return std::make_unique<VarStmt>(std::move(identifier), std::move(initializer));
}

unique_stmt_ptr Parser::whileStatement()
{
    consume(TokenType::LEFT_PAREN, "Expect a '(' after 'while'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect a ')' after 'while'.");
    auto body = statement();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

unique_stmt_ptr Parser::expressionStatement()
{
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_unique<ExprStmt>(std::move(expr));
}

unique_stmt_ptr Parser::function(const std::string& kind)
{
    consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    auto identifier = previous();
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> params;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (params.size() > 254)
            {
                error(peek(), "Can't exceed more than 254 parameters.");
            }

            consume(TokenType::IDENTIFIER, "Expect parameter name.");
            params.emplace_back(previous());
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");

    auto body = block();

    return std::make_unique<FnStmt>(std::move(identifier), std::move(params), std::move(body));
}

std::vector<unique_stmt_ptr> Parser::block()
{
    std::vector<unique_stmt_ptr> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        statements.emplace_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");

    return statements;
}

unique_expr_ptr Parser::assignment()
{
    auto expr = orExpression();

    if (match({TokenType::EQUAL}))
    {
        if (dynamic_cast<VarExpr*>(expr.get()))
        {
            auto value = assignment();
            auto identifier{dynamic_cast<VarExpr*>(expr.release())->identifier};

            return std::make_unique<AssignExpr>(std::move(identifier), std::move(value));
        }

        Error::addError(previous(), "Invalid assignment target.");
    }

    return expr;
}

unique_expr_ptr Parser::orExpression()
{
    auto expr = andExpression();

    while (match({TokenType::OR}))
    {
        auto op = previous();
        auto right = andExpression();
        expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::andExpression()
{
    auto expr = equality();

    while (match({TokenType::AND}))
    {
        auto op = previous();
        auto right = andExpression();
        expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::expression()
{
    return assignment();
}

template <typename Fn>
unique_expr_ptr Parser::binary(Fn func, const std::initializer_list<TokenType>& token_args)
{
    auto expr = func();

    while (match(token_args))
    {
        const auto op = previous();
        auto right = func();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

unique_expr_ptr Parser::equality()
{
    return binary([this]() { return comparison(); },
                  {TokenType::EXCLAMATION_EQUAL, TokenType::EQUAL_EQUAL});
}

unique_expr_ptr Parser::comparison()
{
    return binary([this]() { return term(); }, {TokenType::GREATER, TokenType::GREATER_EQUAL,
                                                TokenType::LESS, TokenType::LESS_EQUAL});
}

unique_expr_ptr Parser::term()
{
    return binary([this]() { return factor(); }, {TokenType::MINUS, TokenType::PLUS});
}

unique_expr_ptr Parser::factor()
{
    return binary([this]() { return unary(); }, {TokenType::SLASH, TokenType::STAR});
}

unique_expr_ptr Parser::unary()
{
    if (match({TokenType::EXCLAMATION, TokenType::MINUS}))
    {
        auto op = previous();
        auto right = unary();

        return std::make_unique<UnaryExpr>(std::move(op), std::move(right));
    }

    return prefix();
}

unique_expr_ptr Parser::prefix()
{
    if (match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS}))
    {
        auto op = previous();
        consume(TokenType::IDENTIFIER,
                "Operators '++' and '--' must be applied to and lvalue operand.");
        auto lvalue = previous();

        if (lvalue.type == TokenType::PLUS_PLUS || lvalue.type == TokenType::MINUS_MINUS)
        {
            throw error(peek(), "Operators '++' and '--' cannot be concatenated.");
        }

        if (op.type == TokenType::PLUS_PLUS)
        {
            return std::make_unique<IncrementExpr>(std::move(lvalue), IncrementExpr::Type::PREFIX);
        }
        else
        {
            return std::make_unique<DecrementExpr>(std::move(lvalue), DecrementExpr::Type::PREFIX);
        }
    }

    return postfix();
}

unique_expr_ptr Parser::postfix()
{
    auto expr = call();

    auto lvalue = previous();

    if (match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS}))
    {
        const auto& op = previous();

        if (lvalue.type != TokenType::IDENTIFIER)
        {
            throw error(lvalue, "Operators '++' and '--' must be applied to and lvalue operand.");
        }

        if (match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS}))
        {
            throw error(lvalue, "Operators '++' and '--' cannot be concatenated.");
        }

        if (op.type == TokenType::PLUS_PLUS)
        {
            expr = std::make_unique<IncrementExpr>(std::move(lvalue), IncrementExpr::Type::POSTFIX);
        }
        else
        {
            expr = std::make_unique<DecrementExpr>(std::move(lvalue), DecrementExpr::Type::POSTFIX);
        }
    }

    return expr;
}

unique_expr_ptr Parser::call()
{
    auto expr = primary();

    while (true)
    {
        if (match({TokenType::LEFT_PAREN}))
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
    std::vector<unique_expr_ptr> arguments;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (arguments.size() > 254)
            {
                error(peek(), "Argument limit exceeded. Can't have more than 254 arguments.");
            }

            arguments.emplace_back(expression());
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RIGHT_PAREN, "Except ')' after arguments.");
    auto paren = previous();
    return std::make_unique<CallExpr>(std::move(callee), std::move(paren), std::move(arguments));
}

// unique_expr_ptr Parser::list()
//{
//     return nullptr;
// }

unique_expr_ptr Parser::primary()
{
    using enum TokenType;
    // if (match({TokenType::LEFT_BRACKET}))
    // {
    //     return list();
    // }
    if (match({NUMBER}))
    {
        return std::make_unique<LiteralExpr>(std::strtod(previous().lexeme.c_str(), nullptr));
    }

    if (match({STRING}))
    {
        return std::make_unique<LiteralExpr>(previous().lexeme);
    }

    if (match({_FALSE}))
    {
        return std::make_unique<LiteralExpr>(false);
    }

    if (match({_TRUE}))
    {
        return std::make_unique<LiteralExpr>(true);
    }

    if (match({NIL}))
    {
        return std::make_unique<LiteralExpr>(std::any{});
    }

    if (match({IDENTIFIER}))
    {
        return std::make_unique<VarExpr>(previous());
    }

    if (match({LEFT_PAREN}))
    {
        auto expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}

bool Parser::match(const std::initializer_list<TokenType> token_args)
{
    bool is_match = std::ranges::any_of(token_args.begin(), token_args.end(),
                                        [this](TokenType x) { return check(x); });
    if (is_match)
    {
        advance();
    }

    return is_match;
}

void Parser::consume(TokenType type, std::string msg)
{
    if (!check(type))
    {
        throw error(peek(), std::move(msg));
    }

    advance();
}

bool Parser::check(TokenType type) const
{
    if (isAtEnd())
    {
        return false;
    }

    return peek().type == type;
}

void Parser::advance()
{
    if (!isAtEnd())
    {
        current++;
    }
}

bool Parser::isAtEnd() const
{
    return peek().type == TokenType::_EOF;
}

const Token& Parser::peek() const
{
    return tokens[current];
}

const Token& Parser::previous() const
{
    return tokens[current - 1];
}

Parser::ParseError Parser::error(const Token& token, std::string msg) const
{
    Error::addError(token, std::move(msg));
    return {};
}

void Parser::synchronize()
{
    advance();
    while (!isAtEnd())
    {
        using enum TokenType;
        if (previous().type == SEMICOLON)
            return;

        switch (peek().type)
        {
        case CLASS:
        case FN:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case PRINT:
        case RETURN:
        case BREAK:
            return;
        default:
            advance();
        }
    }
}